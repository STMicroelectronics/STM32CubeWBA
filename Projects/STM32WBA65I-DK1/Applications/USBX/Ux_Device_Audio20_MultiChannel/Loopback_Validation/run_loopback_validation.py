#!/usr/bin/env python3
"""Record and analyze loopback audio using an existing multi-channel 16-bit WAV reference."""

from __future__ import annotations

import argparse
import contextlib
from functools import lru_cache
import importlib
import io
import math
import re
import sys
import threading
import time
import wave
from array import array
from pathlib import Path
from typing import Iterable, Sequence


SAMPLE_WIDTH_BYTES = 2
MAX_INT16 = 32767
DEFAULT_CAPTURE_TAIL_SECONDS = 0.015
DEFAULT_LEAD_IN_SECONDS = 0.0
DEFAULT_CORRELATION_THRESHOLD = 0.80
DEFAULT_MONITOR_INTERVAL_SECONDS = 0.25
DEFAULT_SAMPLE_START_THRESHOLD = 32
DEFAULT_ACTIVE_LENGTH_TOLERANCE_SECONDS = 0.002
DEFAULT_ADAPTIVE_TAIL_INCREMENT_SECONDS = 0.005
DEFAULT_ADAPTIVE_TAIL_MAX_SECONDS = 0.050
DEFAULT_STREAM_STATE_POLL_INTERVAL_SECONDS = 0.005


def script_directory() -> Path:
    return Path(__file__).resolve().parent


def prepare_output_file(path: Path, label: str) -> Path:
    resolved_path = path.resolve()
    resolved_path.parent.mkdir(parents=True, exist_ok=True)

    if resolved_path.exists():
        if not resolved_path.is_file():
            raise ValueError(f"Expected {label} path to be a file: {resolved_path}")
        resolved_path.unlink()
        print(f"Removed previous {resolved_path.name}")

    return resolved_path


def resolve_existing_file(path_argument: str, label: str) -> Path:
    resolved_path = Path(path_argument).resolve()
    if not resolved_path.exists():
        raise ValueError(f"{label} not found: {resolved_path}")
    if not resolved_path.is_file():
        raise ValueError(f"Expected {label} path to be a file: {resolved_path}")
    return resolved_path


def publish_capture_result(
    args: argparse.Namespace,
    capture_path: Path,
    capture_channels: Sequence[Sequence[int]],
    sample_rate: int,
) -> None:
    if getattr(args, "defer_capture_write", False):
        args._captured_channels = capture_channels
        args._captured_sample_rate = sample_rate
        args._captured_channel_count = len(capture_channels)
        return

    write_wav(capture_path, capture_channels, sample_rate)


def parse_device_argument(value: str):
    stripped_value = value.strip()
    if stripped_value.isdigit():
        return int(stripped_value)
    return stripped_value


def get_first_argument(args: argparse.Namespace, *names: str):
    for name in names:
        value = getattr(args, name, None)
        if value is not None:
            return value
    return None


def import_numpy():
    try:
        np = importlib.import_module("numpy")
    except ImportError as exc:
        raise RuntimeError("Install numpy to use this command: pip install numpy") from exc
    return np


def import_sounddevice():
    try:
        sd = importlib.import_module("sounddevice")
    except ImportError as exc:
        raise RuntimeError("Install sounddevice to use this command: pip install sounddevice") from exc
    return sd


def import_soundcard():
    try:
        sc = importlib.import_module("soundcard")
    except ImportError as exc:
        raise RuntimeError("Install soundcard to use this command: pip install soundcard") from exc
    return sc


def import_miniaudio():
    try:
        miniaudio = importlib.import_module("miniaudio")
    except ImportError as exc:
        raise RuntimeError("Install miniaudio to use this command: pip install miniaudio") from exc
    return miniaudio


def interleave_channels(channels: Sequence[Sequence[int]]) -> array:
    frame_count = len(channels[0])
    interleaved = array("h")

    for sample_index in range(frame_count):
        for channel in channels:
            interleaved.append(int(channel[sample_index]))

    return interleaved


def to_frame_matrix(channels: Sequence[Sequence[int]]):
    np = import_numpy()
    frame_count = len(channels[0])
    matrix = np.zeros((frame_count, len(channels)), dtype=np.int16)

    for channel_index, channel in enumerate(channels):
        matrix[:, channel_index] = np.asarray(channel, dtype=np.int16)

    return matrix


def from_frame_matrix(matrix) -> list[array]:
    np = import_numpy()
    if matrix.ndim != 2:
        raise ValueError("Expected a 2D audio matrix.")
    return [array("h", np.asarray(matrix[:, channel_index], dtype=np.int16).tolist()) for channel_index in range(matrix.shape[1])]


def append_silence(channels: Sequence[Sequence[int]], sample_rate: int, duration_seconds: float) -> list[array]:
    silence_samples = max(0, int(round(sample_rate * duration_seconds)))
    extended = []
    for channel in channels:
        values = array("h", channel)
        if silence_samples != 0:
            values.extend([0] * silence_samples)
        extended.append(values)
    return extended


def prepend_silence(channels: Sequence[Sequence[int]], sample_rate: int, duration_seconds: float) -> list[array]:
    silence_samples = max(0, int(round(sample_rate * duration_seconds)))
    prefixed = []
    for channel in channels:
        values = array("h")
        if silence_samples != 0:
            values.extend([0] * silence_samples)
        values.extend(channel)
        prefixed.append(values)
    return prefixed


def write_wav(path: Path, channels: Sequence[Sequence[int]], sample_rate: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    interleaved = interleave_channels(channels)
    if sys.byteorder != "little":
        interleaved.byteswap()

    with wave.open(str(path), "wb") as wav_file:
        wav_file.setnchannels(len(channels))
        wav_file.setsampwidth(SAMPLE_WIDTH_BYTES)
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(interleaved.tobytes())


def read_wav(path: Path) -> tuple[list[array], int, int]:
    with wave.open(str(path), "rb") as wav_file:
        channels = wav_file.getnchannels()
        sample_rate = wav_file.getframerate()
        sample_width = wav_file.getsampwidth()
        frame_count = wav_file.getnframes()
        raw_frames = wav_file.readframes(frame_count)

    if sample_width != SAMPLE_WIDTH_BYTES:
        raise ValueError(f"Expected 16-bit PCM WAV, got {sample_width * 8}-bit samples.")

    data = array("h")
    data.frombytes(raw_frames)
    if sys.byteorder != "little":
        data.byteswap()

    channel_data = [array("h", data[channel_index::channels]) for channel_index in range(channels)]
    return channel_data, sample_rate, channels


def find_signal_start(
    channels: Sequence[Sequence[int]],
    window_size: int = 256,
    threshold: float = 600.0,
    sample_threshold: int = DEFAULT_SAMPLE_START_THRESHOLD,
) -> int:
    frame_count = len(channels[0])
    if frame_count <= window_size:
        return 0

    coarse_start = 0
    for start in range(0, frame_count - window_size, 32):
        energy = 0.0
        for sample_index in range(start, start + window_size):
            for channel in channels:
                energy += abs(channel[sample_index])

        average = energy / (window_size * len(channels))
        if average >= threshold:
            coarse_start = start
            break

    search_start = max(0, coarse_start - window_size)
    precise_starts = []
    for channel in channels:
        for sample_index in range(search_start, frame_count):
            if abs(channel[sample_index]) > sample_threshold:
                precise_starts.append(sample_index)
                break

    if precise_starts:
        return min(precise_starts)

    return coarse_start


def find_signal_stop(
    channels: Sequence[Sequence[int]],
    sample_threshold: int = DEFAULT_SAMPLE_START_THRESHOLD,
) -> int:
    frame_count = len(channels[0])
    for sample_index in range(frame_count - 1, -1, -1):
        for channel in channels:
            if abs(channel[sample_index]) > sample_threshold:
                return sample_index
    return 0


def normalized_correlation(reference: Sequence[int], capture: Sequence[int]) -> float:
    np = import_numpy()

    reference_values = np.asarray(reference, dtype=np.float64)
    capture_values = np.asarray(capture, dtype=np.float64)
    if (reference_values.size == 0) or (capture_values.size == 0):
        return 0.0

    reference_centered = reference_values - float(np.mean(reference_values))
    capture_centered = capture_values - float(np.mean(capture_values))

    numerator = float(np.dot(reference_centered, capture_centered))
    ref_energy = float(np.dot(reference_centered, reference_centered))
    cap_energy = float(np.dot(capture_centered, capture_centered))
    if (ref_energy == 0.0) or (cap_energy == 0.0):
        return 0.0

    return numerator / math.sqrt(ref_energy * cap_energy)


def rms_level(samples: Sequence[int]) -> float:
    if not samples:
        return 0.0
    return math.sqrt(sum(sample * sample for sample in samples) / len(samples))


def resolve_loopback_input_channel_count(requested_channel_count: int | None, source_channel_count: int) -> int:
    if requested_channel_count is None:
        return source_channel_count
    if requested_channel_count != source_channel_count:
        raise ValueError(
            f"Expected {source_channel_count} input channels to match the reference WAV, got {requested_channel_count}."
        )
    return requested_channel_count


def find_best_channel_mapping(correlation_matrix: Sequence[Sequence[float]]) -> tuple[int, ...]:
    channel_count = len(correlation_matrix)
    if channel_count == 0:
        return tuple()

    @lru_cache(maxsize=None)
    def solve(reference_index: int, used_mask: int) -> tuple[float, tuple[int, ...]]:
        if reference_index == channel_count:
            return 0.0, tuple()

        best_score = float("-inf")
        best_mapping = tuple()
        for capture_index in range(channel_count):
            if used_mask & (1 << capture_index):
                continue

            suffix_score, suffix_mapping = solve(reference_index + 1, used_mask | (1 << capture_index))
            total_score = correlation_matrix[reference_index][capture_index] + suffix_score
            if total_score > best_score:
                best_score = total_score
                best_mapping = (capture_index,) + suffix_mapping

        return best_score, best_mapping

    return solve(0, 0)[1]


def analyze_channel_metrics(
    reference_channels: Sequence[Sequence[int]],
    capture_channels: Sequence[Sequence[int]],
    sample_rate: int,
    threshold: float,
) -> dict:
    channel_count = len(reference_channels)
    reference_start = find_signal_start(reference_channels)
    capture_start = find_signal_start(capture_channels)
    reference_stop = find_signal_stop(reference_channels)
    capture_stop = find_signal_stop(capture_channels)
    settle_samples = int(0.01 * sample_rate)
    compare_length = min(
        sample_rate,
        len(reference_channels[0]) - reference_start - settle_samples,
        len(capture_channels[0]) - capture_start - settle_samples,
    )

    if compare_length < 2048:
        return {
            "reference_start": reference_start,
            "capture_start": capture_start,
            "reference_stop": reference_stop,
            "capture_stop": capture_stop,
            "compare_length": compare_length,
            "insufficient_data": True,
            "passed": False,
            "active_length_pass": False,
            "correlation_pass": False,
            "active_length_tolerance": max(1, int(round(DEFAULT_ACTIVE_LENGTH_TOLERANCE_SECONDS * sample_rate))),
            "reference_active_length": max(0, reference_stop - reference_start + 1),
            "capture_active_length": max(0, capture_stop - capture_start + 1),
            "active_length_delta": abs((reference_stop - reference_start + 1) - (capture_stop - capture_start + 1)),
            "best_mapping": tuple(),
            "correlation_matrix": [],
            "rms_values": [],
            "threshold": threshold,
        }

    correlation_matrix = []
    rms_values = []
    capture_windows = []

    for capture_channel in capture_channels[:channel_count]:
        capture_window = capture_channel[capture_start + settle_samples:capture_start + settle_samples + compare_length]
        capture_windows.append(capture_window)
        rms_values.append(rms_level(capture_window))

    for reference_channel in reference_channels[:channel_count]:
        reference_window = reference_channel[reference_start + settle_samples:reference_start + settle_samples + compare_length]
        row = []
        for capture_window in capture_windows:
            row.append(normalized_correlation(reference_window, capture_window))
        correlation_matrix.append(row)

    best_mapping = find_best_channel_mapping(correlation_matrix)

    active_length_tolerance = max(1, int(round(DEFAULT_ACTIVE_LENGTH_TOLERANCE_SECONDS * sample_rate)))
    reference_active_length = max(0, reference_stop - reference_start + 1)
    capture_active_length = max(0, capture_stop - capture_start + 1)
    active_length_delta = abs(reference_active_length - capture_active_length)

    correlation_pass = True
    for ref_index, cap_index in enumerate(best_mapping):
        if correlation_matrix[ref_index][cap_index] < threshold:
            correlation_pass = False

    active_length_pass = active_length_delta <= active_length_tolerance

    return {
        "reference_start": reference_start,
        "capture_start": capture_start,
        "reference_stop": reference_stop,
        "capture_stop": capture_stop,
        "compare_length": compare_length,
        "insufficient_data": False,
        "passed": active_length_pass and correlation_pass,
        "active_length_pass": active_length_pass,
        "correlation_pass": correlation_pass,
        "active_length_tolerance": active_length_tolerance,
        "reference_active_length": reference_active_length,
        "capture_active_length": capture_active_length,
        "active_length_delta": active_length_delta,
        "best_mapping": best_mapping,
        "correlation_matrix": correlation_matrix,
        "rms_values": rms_values,
        "threshold": threshold,
    }


def print_channel_analysis(metrics: dict) -> int:
    if metrics["insufficient_data"]:
        print("FAIL: not enough recorded data after alignment.")
        return 1

    if not metrics["active_length_pass"]:
        print(
            "FAIL: recorded active signal length does not match the reference "
            f"within {metrics['active_length_tolerance']} samples "
            f"(delta={metrics['active_length_delta']} samples)."
        )
        return 1

    if not metrics["correlation_pass"]:
        print(
            "FAIL: recorded WAV correlation is below the expected threshold "
            f"({metrics['threshold']:.2f})."
        )
        return 1

    if metrics["passed"]:
        print("PASS: recorded WAV matches the expected multi-channel loopback.")
        return 0

    print("FAIL: recorded WAV does not match the expected multi-channel loopback.")
    return 1


def compare_channels(
    reference_channels: Sequence[Sequence[int]],
    capture_channels: Sequence[Sequence[int]],
    sample_rate: int,
    threshold: float,
) -> int:
    metrics = analyze_channel_metrics(reference_channels, capture_channels, sample_rate, threshold)
    return print_channel_analysis(metrics)


def build_alignment_signal(
    channels: Sequence[Sequence[int]],
    start_index: int,
    sample_count: int,
):
    np = import_numpy()
    if sample_count <= 0:
        return np.zeros(0, dtype=np.float64)

    alignment_signal = np.zeros(sample_count, dtype=np.float64)
    contributing_channels = 0

    for channel in channels:
        channel_window = np.asarray(channel[start_index:start_index + sample_count], dtype=np.float64)
        if channel_window.size != sample_count:
            continue

        channel_window = channel_window - float(np.mean(channel_window))
        channel_energy = float(np.dot(channel_window, channel_window))
        if channel_energy <= 0.0:
            continue

        alignment_signal += channel_window / math.sqrt(channel_energy)
        contributing_channels += 1

    if contributing_channels == 0:
        return np.asarray(channels[0][start_index:start_index + sample_count], dtype=np.float64)

    return alignment_signal / float(contributing_channels)


def compute_valid_cross_correlation(search_signal, reference_signal):
    np = import_numpy()

    search_length = int(search_signal.size)
    reference_length = int(reference_signal.size)
    valid_length = search_length - reference_length + 1
    if valid_length <= 0:
        return np.zeros(0, dtype=np.float64)

    fft_size = 1 << (search_length + reference_length - 2).bit_length()
    search_fft = np.fft.rfft(search_signal, fft_size)
    reference_fft = np.fft.rfft(reference_signal[::-1], fft_size)
    full_correlation = np.fft.irfft(search_fft * reference_fft, fft_size)
    return full_correlation[reference_length - 1:reference_length - 1 + valid_length]


def compute_sliding_window_energy(signal, window_length: int):
    np = import_numpy()

    signal_length = int(signal.size)
    if (window_length <= 0) or (signal_length < window_length):
        return np.zeros(0, dtype=np.float64)

    squared_signal = signal * signal
    cumulative_energy = np.empty(signal_length + 1, dtype=np.float64)
    cumulative_energy[0] = 0.0
    np.cumsum(squared_signal, out=cumulative_energy[1:])
    return cumulative_energy[window_length:] - cumulative_energy[:-window_length]


def find_alignment_offset(
    reference_channels: Sequence[Sequence[int]],
    capture_channels: Sequence[Sequence[int]],
    sample_rate: int,
) -> int:
    np = import_numpy()

    reference_length = len(reference_channels[0])
    capture_length = len(capture_channels[0])
    if capture_length <= reference_length:
        return 0

    reference_start = find_signal_start(reference_channels)
    max_offset = capture_length - reference_length
    alignment_window = min(reference_length - reference_start, capture_length, sample_rate)
    if alignment_window < 2048:
        return max(0, min(max_offset, find_signal_start(capture_channels) - reference_start))

    reference_signal = build_alignment_signal(reference_channels, reference_start, alignment_window)
    capture_signal = build_alignment_signal(capture_channels, 0, capture_length)

    correlation_values = compute_valid_cross_correlation(capture_signal, reference_signal)
    reference_energy = float(np.dot(reference_signal, reference_signal))
    capture_energy = compute_sliding_window_energy(capture_signal, alignment_window)
    denominator = np.sqrt(np.maximum(reference_energy * capture_energy, 1e-12))
    normalized_scores = correlation_values / denominator

    best_window_start = int(np.argmax(normalized_scores))
    return max(0, min(max_offset, best_window_start - reference_start))


def trim_capture_to_reference(
    reference_channels: Sequence[Sequence[int]],
    capture_channels: Sequence[Sequence[int]],
    sample_rate: int,
) -> tuple[list[array], int, int]:
    channel_count = len(reference_channels)
    reference_length = len(reference_channels[0])
    trim_start = find_alignment_offset(reference_channels, capture_channels, sample_rate)
    trim_stop = trim_start + reference_length

    trimmed_channels = []
    for capture_channel in capture_channels[:channel_count]:
        trimmed = array("h", capture_channel[trim_start:min(trim_stop, len(capture_channel))])
        if len(trimmed) < reference_length:
            trimmed.extend([0] * (reference_length - len(trimmed)))
        trimmed_channels.append(trimmed)

    return trimmed_channels, trim_start, trim_stop


def load_reference_wav(reference_path: str) -> tuple[list[array], int]:
    resolved_reference_path = resolve_existing_file(reference_path, "Reference WAV")
    reference_channels, reference_sample_rate, reference_channel_count = read_wav(resolved_reference_path)
    if reference_channel_count <= 0:
        raise ValueError("Reference WAV must contain at least one audio channel.")
    return reference_channels, reference_sample_rate


def ensure_reference_source(args: argparse.Namespace) -> Path:
    source_argument = get_first_argument(args, "input_file", "source")
    return resolve_existing_file(source_argument, "Reference WAV")


def matches_device_name(candidate_name: str, requested_name: str) -> bool:
    def normalize(name: str) -> str:
        normalized = name.strip().lower()
        normalized = re.sub(r"\(\s*\d+\s*-\s*", "(", normalized)
        normalized = re.sub(r"[^a-z0-9]+", " ", normalized)
        return normalized.strip()

    candidate = normalize(candidate_name)
    requested = normalize(requested_name)
    return (candidate == requested) or (requested in candidate) or (candidate in requested)


def find_soundcard_microphone(device_argument):
    sc = import_soundcard()
    microphones = list(sc.all_microphones(include_loopback=True))
    if not microphones:
        raise RuntimeError("No soundcard microphones were found.")

    if device_argument is None:
        return sc.default_microphone()

    if isinstance(device_argument, int):
        if 0 <= device_argument < len(microphones):
            return microphones[device_argument]

        sd = import_sounddevice()
        devices = sd.query_devices()
        if 0 <= device_argument < len(devices):
            requested_name = str(devices[device_argument]["name"])
            for microphone in microphones:
                if matches_device_name(microphone.name, requested_name):
                    return microphone

        raise ValueError(f"No soundcard microphone matches device index {device_argument}.")

    for microphone in microphones:
        if matches_device_name(microphone.name, str(device_argument)):
            return microphone

    raise ValueError(f"No soundcard microphone matches '{device_argument}'.")


def find_miniaudio_device_id(device_argument, device_list, stream_label: str):
    if not device_list:
        raise RuntimeError(f"No miniaudio {stream_label} devices were found.")

    if device_argument is None:
        return device_list[0]["id"]

    if isinstance(device_argument, int):
        if 0 <= device_argument < len(device_list):
            return device_list[device_argument]["id"]

        sd = import_sounddevice()
        devices = sd.query_devices()
        if 0 <= device_argument < len(devices):
            requested_name = str(devices[device_argument]["name"])
            for device in device_list:
                if matches_device_name(device["name"], requested_name):
                    return device["id"]

        raise ValueError(f"No miniaudio {stream_label} device matches index {device_argument}.")

    for device in device_list:
        if matches_device_name(device["name"], str(device_argument)):
            return device["id"]

    raise ValueError(f"No miniaudio {stream_label} device matches '{device_argument}'.")


def find_sounddevice_device_index(device_argument, stream_label: str, preferred_hostapi: str | None = None) -> int:
    sd = import_sounddevice()
    devices = sd.query_devices()
    host_apis = sd.query_hostapis()
    channel_key = "max_input_channels" if stream_label == "input" else "max_output_channels"
    preferred_hostapi_name = preferred_hostapi.strip().lower() if preferred_hostapi else None

    def can_stream(device) -> bool:
        return int(device[channel_key]) > 0

    def hostapi_name(device) -> str:
        return str(host_apis[device["hostapi"]]["name"])

    def ranked_matches(requested_name: str) -> list[int]:
        requested = requested_name.strip().lower()
        matches = [
            index
            for index, device in enumerate(devices)
            if can_stream(device) and matches_device_name(str(device["name"]), requested_name)
        ]
        if not matches:
            return []

        return sorted(
            matches,
            key=lambda index: (
                0 if (preferred_hostapi_name and hostapi_name(devices[index]).lower() == preferred_hostapi_name) else 1,
                0 if str(devices[index]["name"]).strip().lower() == requested else 1,
                index,
            ),
        )

    if device_argument is None:
        default_index = int(sd.default.device[0 if stream_label == "input" else 1])
        if (0 <= default_index < len(devices)) and can_stream(devices[default_index]):
            default_name = str(devices[default_index]["name"])
            if preferred_hostapi_name and (hostapi_name(devices[default_index]).lower() != preferred_hostapi_name):
                matches = ranked_matches(default_name)
                if matches:
                    return matches[0]
            return default_index
        raise ValueError(f"No default sounddevice {stream_label} device is available.")

    if isinstance(device_argument, int):
        if not (0 <= device_argument < len(devices)):
            raise ValueError(f"No sounddevice {stream_label} device matches index {device_argument}.")
        if not can_stream(devices[device_argument]):
            raise ValueError(f"Sounddevice device index {device_argument} cannot be used as {stream_label}.")

        if preferred_hostapi_name and (hostapi_name(devices[device_argument]).lower() != preferred_hostapi_name):
            matches = ranked_matches(str(devices[device_argument]["name"]))
            if matches:
                return matches[0]
        return device_argument

    matches = ranked_matches(str(device_argument))
    if matches:
        return matches[0]

    raise ValueError(f"No sounddevice {stream_label} device matches '{device_argument}'.")


def resolve_post_playback_margin_seconds() -> tuple[float, str]:
    return 0.0, "standard path (no extra margin)"


def command_devices(_: argparse.Namespace) -> int:
    sd = import_sounddevice()
    devices = sd.query_devices()
    host_apis = sd.query_hostapis()

    print("Available audio devices:")
    for index, device in enumerate(devices):
        host_api_name = host_apis[device["hostapi"]]["name"]
        print(
            f"[{index}] {device['name']} | in={device['max_input_channels']} "
            f"out={device['max_output_channels']} | hostapi={host_api_name}"
        )

    try:
        sc = import_soundcard()
        print("\nSoundcard microphones:")
        for index, microphone in enumerate(sc.all_microphones(include_loopback=True)):
            print(f"[{index}] {microphone.name} | channels={microphone.channels}")
    except RuntimeError:
        pass

    try:
        miniaudio = import_miniaudio()
        ma_devices = miniaudio.Devices()
        print("\nMiniaudio playbacks:")
        for index, playback in enumerate(ma_devices.get_playbacks()):
            print(f"[{index}] {playback['name']}")
        print("Miniaudio recording devices:")
        for index, capture in enumerate(ma_devices.get_captures()):
            print(f"[{index}] {capture['name']}")
    except RuntimeError:
        pass

    return 0


def command_playrec(args: argparse.Namespace) -> int:
    sd = import_sounddevice()
    np = import_numpy()

    source_path = Path(get_first_argument(args, "source", "input_file")).resolve()
    capture_path = prepare_output_file(Path(get_first_argument(args, "capture", "output_file")), "recording")
    source_channels, source_sample_rate, source_channel_count = read_wav(source_path)
    effective_sample_rate = args.sample_rate if args.sample_rate is not None else source_sample_rate
    effective_input_channels = resolve_loopback_input_channel_count(args.input_channels, source_channel_count)

    if source_sample_rate != effective_sample_rate:
        print(f"FAIL: expected {effective_sample_rate} Hz source, got {source_sample_rate} Hz.")
        return 1

    playback_channels = prepend_silence(source_channels, effective_sample_rate, args.lead_in_seconds)
    playback_channels = append_silence(playback_channels, effective_sample_rate, args.tail_seconds)
    playback_matrix = to_frame_matrix(playback_channels)
    capture_matrix = np.zeros((playback_matrix.shape[0], effective_input_channels), dtype=np.int16)
    playback_cursor = 0
    capture_cursor = 0
    output_callback_calls = 0
    captured_frames = 0
    monitor_frame_count = max(1, int(round(args.monitor_interval * effective_sample_rate)))
    monitor_frames = 0
    monitor_sum_squares = np.zeros(effective_input_channels, dtype=np.float64)
    source_rms = [round(rms_level(channel), 1) for channel in source_channels]

    print(f"Play source  : {source_path}")
    print(f"Recording file : {capture_path}")
    print(f"Input device : {args.input_device or 'default'}")
    print(f"Output device: {args.output_device or 'default'}")
    print(f"Input backend: {args.input_backend}")
    print(f"Format       : {effective_sample_rate} Hz, {source_channel_count} channels, 16-bit PCM")
    print("Start playback + recording...")

    if args.input_backend == "sounddevice-wdmks":
        input_device_index = find_sounddevice_device_index(args.input_device, "input", preferred_hostapi="Windows WDM-KS")
        output_device_index = find_sounddevice_device_index(args.output_device, "output", preferred_hostapi="Windows WDM-KS")
        post_playback_margin_seconds, post_playback_margin_reason = resolve_post_playback_margin_seconds()
        post_playback_margin_frames = int(round(post_playback_margin_seconds * effective_sample_rate))
        capture_matrix = np.zeros((playback_matrix.shape[0] + post_playback_margin_frames, effective_input_channels), dtype=np.int16)

        sd.check_input_settings(
            device=input_device_index,
            samplerate=effective_sample_rate,
            channels=effective_input_channels,
            dtype="int16",
        )
        sd.check_output_settings(
            device=output_device_index,
            samplerate=effective_sample_rate,
            channels=playback_matrix.shape[1],
            dtype="int16",
        )

        playback_data_queued_time = None
        playback_stream_drained_time = None
        playback_data_queued_event = threading.Event()
        playback_stream_drained_event = threading.Event()

        def input_callback(indata, frames, time_info, status):
            nonlocal capture_cursor, captured_frames, monitor_frames
            del frames, time_info

            if status:
                print(f"Input stream status: {status}")

            valid_frames = min(indata.shape[0], capture_matrix.shape[0] - capture_cursor)
            if valid_frames <= 0:
                return

            capture_matrix[capture_cursor:capture_cursor + valid_frames] = indata[:valid_frames]
            capture_cursor += valid_frames
            captured_frames += valid_frames

            if args.monitor_rms:
                monitor_chunk = np.asarray(indata[:valid_frames], dtype=np.float64)
                monitor_sum_squares[:monitor_chunk.shape[1]] += np.sum(monitor_chunk * monitor_chunk, axis=0)
                monitor_frames += valid_frames

                if monitor_frames >= monitor_frame_count:
                    channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
                    channel_report = " ".join(
                        f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
                        for channel_index in range(effective_input_channels)
                    )
                    print(f"Input RMS: {channel_report}", flush=True)
                    monitor_frames = 0
                    monitor_sum_squares[:] = 0.0

        def output_callback(outdata, frames, time_info, status):
            nonlocal playback_cursor, output_callback_calls, playback_data_queued_time
            del time_info
            output_callback_calls += 1

            if status:
                print(f"Output stream status: {status}")

            next_cursor = min(playback_cursor + frames, playback_matrix.shape[0])
            valid_frames = next_cursor - playback_cursor

            if valid_frames > 0:
                outdata[:valid_frames] = playback_matrix[playback_cursor:next_cursor]
            if valid_frames < frames:
                outdata[valid_frames:] = 0

            playback_cursor = next_cursor

            if playback_cursor >= playback_matrix.shape[0]:
                if not playback_data_queued_event.is_set():
                    playback_data_queued_time = time.perf_counter()
                    playback_data_queued_event.set()
                raise sd.CallbackStop()

        def output_finished_callback():
            nonlocal playback_stream_drained_time
            playback_stream_drained_time = time.perf_counter()
            playback_stream_drained_event.set()

        input_stream = sd.InputStream(
            samplerate=effective_sample_rate,
            blocksize=0,
            device=input_device_index,
            channels=effective_input_channels,
            dtype="int16",
            latency="low",
            callback=input_callback,
        )
        output_stream = sd.OutputStream(
            samplerate=effective_sample_rate,
            blocksize=0,
            device=output_device_index,
            channels=playback_matrix.shape[1],
            dtype="int16",
            latency="low",
            callback=output_callback,
            finished_callback=output_finished_callback,
            prime_output_buffers_using_stream_callback=True,
        )

        try:
            input_stream.start()
            output_stream.start()
            playback_timeout_seconds = (playback_matrix.shape[0] / effective_sample_rate) + 2.0
            if not playback_stream_drained_event.wait(timeout=playback_timeout_seconds):
                raise RuntimeError("Timed out while waiting for the playback stream to drain.")
        finally:
            if not output_stream.stopped:
                output_stream.stop()
            if not input_stream.stopped:
                input_stream.stop()
            output_stream.close()
            input_stream.close()

        if args.monitor_rms and (monitor_frames > 0):
            channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
            channel_report = " ".join(
                f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
                for channel_index in range(effective_input_channels)
            )
            print(f"Input RMS: {channel_report}", flush=True)

        capture_slice = capture_matrix[:capture_cursor] if capture_cursor > 0 else capture_matrix[:0]
        capture_channels = from_frame_matrix(capture_slice) if capture_cursor > 0 else [array("h") for _ in range(effective_input_channels)]
        publish_capture_result(args, capture_path, capture_channels, effective_sample_rate)

        if capture_cursor == 0:
            capture_rms = [0.0] * effective_input_channels
        else:
            capture_rms = [
                round(float(np.sqrt(np.mean(capture_slice[:, channel_index].astype(np.float64) ** 2))), 1)
                for channel_index in range(capture_slice.shape[1])
            ]

        print("Recording complete.")
        return 0

    if args.input_backend == "sounddevice-wasapi-exclusive":
        if sys.platform != "win32":
            raise RuntimeError("The sounddevice-wasapi-exclusive backend is only available on Windows.")
        if not hasattr(sd, "WasapiSettings"):
            raise RuntimeError("This sounddevice build does not expose WASAPI settings.")

        input_device_index = find_sounddevice_device_index(args.input_device, "input", preferred_hostapi="Windows WASAPI")
        output_device_index = find_sounddevice_device_index(args.output_device, "output", preferred_hostapi="Windows WASAPI")
        input_settings = sd.WasapiSettings(exclusive=True, auto_convert=False, explicit_sample_format=True)
        output_settings = sd.WasapiSettings(exclusive=True, auto_convert=False, explicit_sample_format=True)

        sd.check_input_settings(
            device=input_device_index,
            samplerate=effective_sample_rate,
            channels=effective_input_channels,
            dtype="int16",
            extra_settings=input_settings,
        )
        sd.check_output_settings(
            device=output_device_index,
            samplerate=effective_sample_rate,
            channels=playback_matrix.shape[1],
            dtype="int16",
            extra_settings=output_settings,
        )

        def output_callback(outdata, frames, time_info, status):
            nonlocal playback_cursor, output_callback_calls
            del time_info
            output_callback_calls += 1

            if status:
                print(f"Output stream status: {status}")

            next_cursor = min(playback_cursor + frames, playback_matrix.shape[0])
            valid_playback_frames = next_cursor - playback_cursor

            if valid_playback_frames > 0:
                outdata[:valid_playback_frames] = playback_matrix[playback_cursor:next_cursor]
            if valid_playback_frames < frames:
                outdata[valid_playback_frames:] = 0

            playback_cursor = next_cursor

            if playback_cursor >= playback_matrix.shape[0]:
                raise sd.CallbackStop()

        input_stream = sd.InputStream(
            samplerate=effective_sample_rate,
            blocksize=0,
            device=input_device_index,
            channels=effective_input_channels,
            dtype="int16",
            latency="low",
            extra_settings=input_settings,
        )
        output_stream = sd.OutputStream(
            samplerate=effective_sample_rate,
            blocksize=0,
            device=output_device_index,
            channels=playback_matrix.shape[1],
            dtype="int16",
            latency="low",
            extra_settings=output_settings,
            callback=output_callback,
            prime_output_buffers_using_stream_callback=True,
        )

        try:
            input_stream.start()
            output_stream.start()
            timeout_ms = int(math.ceil((playback_matrix.shape[0] / effective_sample_rate) * 1000.0)) + 2000
            elapsed_ms = 0
            read_block_frames = 1024
            while (output_stream.active or (capture_cursor < capture_matrix.shape[0])) and (elapsed_ms < timeout_ms):
                if capture_cursor < capture_matrix.shape[0]:
                    frames_to_read = min(read_block_frames, capture_matrix.shape[0] - capture_cursor)
                    indata, overflowed = input_stream.read(frames_to_read)
                    if overflowed:
                        print("Input stream status: overflow")

                    valid_frames = indata.shape[0]
                    if valid_frames > 0:
                        next_cursor = capture_cursor + valid_frames
                        capture_matrix[capture_cursor:next_cursor, :indata.shape[1]] = indata
                        captured_frames += valid_frames

                        if args.monitor_rms:
                            monitor_chunk = np.asarray(indata, dtype=np.float64)
                            monitor_sum_squares[:indata.shape[1]] += np.sum(monitor_chunk * monitor_chunk, axis=0)
                            monitor_frames += valid_frames

                            if monitor_frames >= monitor_frame_count:
                                channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
                                channel_report = " ".join(
                                    f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
                                    for channel_index in range(effective_input_channels)
                                )
                                print(f"Input RMS: {channel_report}", flush=True)
                                monitor_frames = 0
                                monitor_sum_squares[:] = 0.0

                        capture_cursor = next_cursor

                sd.sleep(10)
                elapsed_ms += 10
        finally:
            if not output_stream.stopped:
                output_stream.stop()
            if not input_stream.stopped:
                input_stream.stop()
            output_stream.close()
            input_stream.close()

        if args.monitor_rms and (monitor_frames > 0):
            channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
            channel_report = " ".join(
                f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
                for channel_index in range(effective_input_channels)
            )
            print(f"Input RMS: {channel_report}", flush=True)

        capture_slice = capture_matrix[:capture_cursor] if capture_cursor > 0 else capture_matrix[:0]
        capture_channels = from_frame_matrix(capture_slice) if capture_cursor > 0 else [array("h") for _ in range(effective_input_channels)]
        publish_capture_result(args, capture_path, capture_channels, effective_sample_rate)
        print("Recording complete.")
        return 0

    if args.input_backend == "miniaudio":
        miniaudio = import_miniaudio()
        ma_devices = miniaudio.Devices()
        capture_chunks = []
        stop_capture = threading.Event()

        input_id = find_miniaudio_device_id(args.input_device, ma_devices.get_captures(), "recording")
        output_id = find_miniaudio_device_id(args.output_device, ma_devices.get_playbacks(), "playback")

        def playback_generator():
            nonlocal playback_cursor, output_callback_calls
            requested_frames = yield b""
            while True:
                output_callback_calls += 1
                frames = int(requested_frames)
                next_cursor = min(playback_cursor + frames, playback_matrix.shape[0])
                chunk = playback_matrix[playback_cursor:next_cursor]

                if chunk.shape[0] < frames:
                    pad = np.zeros((frames - chunk.shape[0], playback_matrix.shape[1]), dtype=np.int16)
                    chunk = np.vstack((chunk, pad))

                playback_cursor = next_cursor
                requested_frames = yield chunk

        def capture_generator():
            nonlocal capture_cursor, captured_frames, monitor_frames
            while not stop_capture.is_set():
                raw = yield
                if not raw:
                    continue

                capture_chunks.append(bytes(raw))
                values = np.frombuffer(raw, dtype=np.int16)
                if values.size == 0:
                    continue

                values = values[: values.size - (values.size % effective_input_channels)]
                if values.size == 0:
                    continue

                frames = values.reshape(-1, effective_input_channels)
                valid_frames = frames.shape[0]
                captured_frames += valid_frames
                capture_cursor += valid_frames

                if args.monitor_rms:
                    monitor_chunk = frames.astype(np.float64)
                    monitor_sum_squares[:frames.shape[1]] += np.sum(monitor_chunk * monitor_chunk, axis=0)
                    monitor_frames += valid_frames

                    if monitor_frames >= monitor_frame_count:
                        channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
                        channel_report = " ".join(
                            f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
                            for channel_index in range(effective_input_channels)
                        )
                        print(f"Input RMS: {channel_report}", flush=True)
                        monitor_frames = 0
                        monitor_sum_squares[:] = 0.0

        play_gen = playback_generator()
        next(play_gen)
        cap_gen = capture_generator()
        next(cap_gen)

        playback_device = miniaudio.PlaybackDevice(
            output_format=miniaudio.SampleFormat.SIGNED16,
            nchannels=playback_matrix.shape[1],
            sample_rate=effective_sample_rate,
            device_id=output_id,
        )
        capture_device = miniaudio.CaptureDevice(
            input_format=miniaudio.SampleFormat.SIGNED16,
            nchannels=effective_input_channels,
            sample_rate=effective_sample_rate,
            device_id=input_id,
        )

        try:
            capture_device.start(cap_gen)
            playback_device.start(play_gen)
            time.sleep((playback_matrix.shape[0] / effective_sample_rate) + 2.0)
        finally:
            stop_capture.set()
            playback_device.stop()
            capture_device.stop()
            playback_device.close()
            capture_device.close()

        joined = b"".join(capture_chunks)
        values = np.frombuffer(joined, dtype=np.int16)
        if values.size == 0:
            capture_matrix = np.zeros((0, effective_input_channels), dtype=np.int16)
        else:
            values = values[: values.size - (values.size % effective_input_channels)]
            capture_matrix = values.reshape(-1, effective_input_channels)

        if args.monitor_rms and (monitor_frames > 0):
            channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
            channel_report = " ".join(
                f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
                for channel_index in range(effective_input_channels)
            )
            print(f"Input RMS: {channel_report}", flush=True)

        capture_channels = from_frame_matrix(capture_matrix) if capture_matrix.size else [array("h") for _ in range(effective_input_channels)]
        publish_capture_result(args, capture_path, capture_channels, effective_sample_rate)
        print("Recording complete.")
        return 0

    sd.check_output_settings(device=args.output_device, samplerate=effective_sample_rate, channels=playback_matrix.shape[1], dtype="int16")

    if args.input_backend == "sounddevice":
        sd.check_input_settings(device=args.input_device, samplerate=effective_sample_rate, channels=effective_input_channels, dtype="int16")

    def output_callback(outdata, frames, time, status):
        nonlocal playback_cursor, output_callback_calls
        del time
        output_callback_calls += 1

        if status:
            print(f"Output stream status: {status}")

        next_cursor = min(playback_cursor + frames, playback_matrix.shape[0])
        valid_frames = next_cursor - playback_cursor

        if valid_frames > 0:
            outdata[:valid_frames] = playback_matrix[playback_cursor:next_cursor]

        if valid_frames < frames:
            outdata[valid_frames:] = 0

        playback_cursor = next_cursor
        if playback_cursor >= playback_matrix.shape[0]:
            raise sd.CallbackStop()

    output_stream = sd.OutputStream(
        samplerate=effective_sample_rate,
        device=args.output_device,
        channels=playback_matrix.shape[1],
        dtype="int16",
        callback=output_callback,
    )

    if args.input_backend == "soundcard":
        soundcard_capture_error = None
        capture_done = threading.Event()
        microphone = find_soundcard_microphone(args.input_device)

        def capture_worker() -> None:
            nonlocal capture_cursor, captured_frames, soundcard_capture_error, monitor_frames
            com_initialized = False
            try:
                if sys.platform == "win32":
                    import ctypes

                    com_result = ctypes.windll.ole32.CoInitializeEx(None, 0x2)
                    com_initialized = com_result in (0, 1)

                channel_map = list(range(effective_input_channels))
                block_frames = 1024
                with microphone.recorder(samplerate=effective_sample_rate, channels=channel_map) as recorder:
                    while capture_cursor < capture_matrix.shape[0]:
                        frames_to_read = min(block_frames, capture_matrix.shape[0] - capture_cursor)
                        indata = recorder.record(numframes=frames_to_read)
                        if indata.size == 0:
                            continue

                        valid_frames = min(frames_to_read, indata.shape[0])
                        chunk = np.clip(np.round(indata[:valid_frames] * MAX_INT16), -32768, 32767).astype(np.int16)
                        next_cursor = capture_cursor + valid_frames
                        capture_matrix[capture_cursor:next_cursor, :chunk.shape[1]] = chunk
                        captured_frames += valid_frames

                        if args.monitor_rms:
                            monitor_chunk = chunk.astype(np.float64)
                            monitor_sum_squares[:chunk.shape[1]] += np.sum(monitor_chunk * monitor_chunk, axis=0)
                            monitor_frames += valid_frames

                            if monitor_frames >= monitor_frame_count:
                                channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
                                channel_report = " ".join(
                                    f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
                                    for channel_index in range(effective_input_channels)
                                )
                                print(f"Input RMS: {channel_report}", flush=True)
                                monitor_frames = 0
                                monitor_sum_squares[:] = 0.0

                        capture_cursor = next_cursor
            except Exception as exc:
                soundcard_capture_error = exc
            finally:
                if com_initialized:
                    ctypes.windll.ole32.CoUninitialize()
                capture_done.set()

        capture_thread = threading.Thread(target=capture_worker, name="soundcard-capture", daemon=True)

        try:
            capture_thread.start()
            output_stream.start()
            timeout_ms = int(math.ceil((playback_matrix.shape[0] / effective_sample_rate) * 1000.0)) + 2000
            elapsed_ms = 0
            while output_stream.active and (elapsed_ms < timeout_ms):
                sd.sleep(10)
                elapsed_ms += 10
        finally:
            if not output_stream.stopped:
                output_stream.stop()
            output_stream.close()

        capture_timeout_seconds = (playback_matrix.shape[0] / effective_sample_rate) + 2.0
        capture_thread.join(timeout=capture_timeout_seconds)
        if soundcard_capture_error is not None:
            raise RuntimeError(f"Soundcard recording failed: {soundcard_capture_error}")
        if capture_thread.is_alive() and not capture_done.is_set():
            raise RuntimeError("Soundcard recording did not complete before timeout.")
    else:
        input_stream = sd.InputStream(
            samplerate=effective_sample_rate,
            device=args.input_device,
            channels=effective_input_channels,
            dtype="int16",
        )

        try:
            input_stream.start()
            output_stream.start()
            timeout_ms = int(math.ceil((playback_matrix.shape[0] / effective_sample_rate) * 1000.0)) + 2000
            elapsed_ms = 0
            read_block_frames = 1024
            while (output_stream.active or (capture_cursor < capture_matrix.shape[0])) and (elapsed_ms < timeout_ms):
                if capture_cursor < capture_matrix.shape[0]:
                    frames_to_read = min(read_block_frames, capture_matrix.shape[0] - capture_cursor)
                    indata, overflowed = input_stream.read(frames_to_read)
                    if overflowed:
                        print("Input stream status: overflow")

                    valid_frames = indata.shape[0]
                    if valid_frames > 0:
                        next_cursor = capture_cursor + valid_frames
                        capture_matrix[capture_cursor:next_cursor, :indata.shape[1]] = indata
                        captured_frames += valid_frames

                        if args.monitor_rms:
                            monitor_chunk = np.asarray(indata, dtype=np.float64)
                            monitor_sum_squares[:indata.shape[1]] += np.sum(monitor_chunk * monitor_chunk, axis=0)
                            monitor_frames += valid_frames

                            if monitor_frames >= monitor_frame_count:
                                channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
                                channel_report = " ".join(
                                    f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
                                    for channel_index in range(effective_input_channels)
                                )
                                print(f"Input RMS: {channel_report}", flush=True)
                                monitor_frames = 0
                                monitor_sum_squares[:] = 0.0

                        capture_cursor = next_cursor

                sd.sleep(10)
                elapsed_ms += 10
        finally:
            if not output_stream.stopped:
                output_stream.stop()
            if not input_stream.stopped:
                input_stream.stop()
            output_stream.close()
            input_stream.close()

    if args.monitor_rms and (monitor_frames > 0):
        channel_rms = np.sqrt(monitor_sum_squares / monitor_frames)
        channel_report = " ".join(
            f"CH{channel_index + 1}={channel_rms[channel_index]:.1f}"
            for channel_index in range(effective_input_channels)
        )
        print(f"Input RMS: {channel_report}", flush=True)

    if capture_cursor == 0:
        capture_channels = from_frame_matrix(capture_matrix)
    else:
        capture_channels = from_frame_matrix(capture_matrix[:capture_cursor])
    publish_capture_result(args, capture_path, capture_channels, effective_sample_rate)

    print("Recording complete.")
    return 0


def command_validate(args: argparse.Namespace) -> int:
    source_path = ensure_reference_source(args)
    capture_path = Path(get_first_argument(args, "output_file", "capture")).resolve()
    reference_channels, reference_sample_rate, reference_channel_count = read_wav(source_path)
    validation_sample_rate = reference_sample_rate
    validation_channel_count = resolve_loopback_input_channel_count(args.input_channels, reference_channel_count)

    current_tail_seconds = args.tail_seconds
    max_tail_seconds = max(DEFAULT_ADAPTIVE_TAIL_MAX_SECONDS, current_tail_seconds)
    analyze_status = 1
    analysis_metrics = None
    final_attempt_output = ""

    print("Recording in progress...", flush=True)
    while True:
        playrec_args = argparse.Namespace(
            source=str(source_path),
            capture=str(capture_path),
            sample_rate=validation_sample_rate,
            input_device=args.input_device,
            output_device=args.output_device,
            input_channels=validation_channel_count,
            input_backend=args.input_backend,
            lead_in_seconds=DEFAULT_LEAD_IN_SECONDS,
            tail_seconds=current_tail_seconds,
            monitor_rms=args.monitor_rms,
            monitor_interval=args.monitor_interval,
            defer_capture_write=True,
        )
        attempt_output = io.StringIO()
        try:
            with contextlib.redirect_stdout(attempt_output):
                playrec_status = command_playrec(playrec_args)
        except Exception:
            print(attempt_output.getvalue(), end="")
            raise

        final_attempt_output = attempt_output.getvalue()
        if playrec_status != 0:
            print(final_attempt_output, end="")
            return playrec_status

        capture_channels = getattr(playrec_args, "_captured_channels", None)
        capture_sample_rate = getattr(playrec_args, "_captured_sample_rate", None)
        capture_channel_count = getattr(playrec_args, "_captured_channel_count", None)

        if (capture_channels is None) or (capture_sample_rate is None) or (capture_channel_count is None):
            capture_channels, capture_sample_rate, capture_channel_count = read_wav(capture_path)

        if capture_channel_count != validation_channel_count:
            raise ValueError(f"Expected {validation_channel_count} recorded channels, got {capture_channel_count}.")
        if capture_sample_rate != validation_sample_rate:
            raise ValueError(f"Expected {validation_sample_rate} Hz recording, got {capture_sample_rate} Hz.")

        trimmed_capture_channels, trim_start, trim_stop = trim_capture_to_reference(reference_channels, capture_channels, validation_sample_rate)
        write_wav(capture_path, trimmed_capture_channels, validation_sample_rate)

        analysis_metrics = analyze_channel_metrics(reference_channels, trimmed_capture_channels, validation_sample_rate, args.threshold)
        analyze_status = 0 if analysis_metrics["passed"] else 1

        retryable_active_length_failure = (
            (analyze_status != 0)
            and (not analysis_metrics["insufficient_data"])
            and (not analysis_metrics["active_length_pass"])
            and (analysis_metrics["capture_active_length"] < analysis_metrics["reference_active_length"])
        )

        if not retryable_active_length_failure:
            break

        next_tail_seconds = round(current_tail_seconds + DEFAULT_ADAPTIVE_TAIL_INCREMENT_SECONDS, 6)
        if next_tail_seconds > max_tail_seconds:
            break

        current_tail_seconds = next_tail_seconds

    print(final_attempt_output, end="")
    print("Analyzing recording...")
    analyze_status = print_channel_analysis(analysis_metrics)
    print(f"Recording file : {capture_path}")

    return analyze_status


def command_analyze(args: argparse.Namespace) -> int:
    capture_path = Path(get_first_argument(args, "capture", "output_file")).resolve()
    capture_channels, capture_sample_rate, capture_channel_count = read_wav(capture_path)
    reference_channels, reference_sample_rate = load_reference_wav(args.reference)
    reference_channel_count = len(reference_channels)

    if capture_channel_count != reference_channel_count:
        print(f"FAIL: expected {reference_channel_count} recorded channels, got {capture_channel_count}.")
        return 1
    if capture_sample_rate != reference_sample_rate:
        print(f"FAIL: expected {reference_sample_rate} Hz recording, got {capture_sample_rate} Hz.")
        return 1

    return compare_channels(reference_channels, capture_channels, reference_sample_rate, args.threshold)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Record and analyze USB audio loopback from an existing 16-bit PCM WAV reference. No synthetic reference is generated automatically."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    devices_parser = subparsers.add_parser("devices", help="List local audio input and output devices.")
    devices_parser.set_defaults(func=command_devices)

    playrec_parser = subparsers.add_parser("playrec", help="Play an existing WAV to the STM32 output stream and record the STM32 input stream.")
    playrec_parser.add_argument("source", help="Existing source WAV file sent to USB OUT.")
    playrec_parser.add_argument("capture", help="Recorded WAV file saved from USB IN.")
    playrec_parser.add_argument("--sample-rate", type=int, help="Expected sample rate in Hz. If omitted, uses the source WAV sample rate.")
    playrec_parser.add_argument("--input-device", type=parse_device_argument, help="Input device name or index.")
    playrec_parser.add_argument("--output-device", type=parse_device_argument, help="Output device name or index.")
    playrec_parser.add_argument("--input-channels", type=int, help="Number of input channels to record. If omitted, uses the source WAV channel count.")
    playrec_parser.add_argument("--input-backend", choices=("sounddevice", "sounddevice-wasapi-exclusive", "sounddevice-wdmks", "soundcard", "miniaudio"), default="sounddevice", help="Audio backend used for recording. sounddevice-wdmks selects the Windows WDM-KS host API through PortAudio; sounddevice-wasapi-exclusive selects exclusive WASAPI. The sounddevice-wasapi-exclusive, sounddevice-wdmks, and miniaudio backends also handle playback.")
    playrec_parser.add_argument("--lead-in-seconds", type=float, default=0.0, help="Initial silence inserted before playback starts.")
    playrec_parser.add_argument("--tail-seconds", type=float, default=DEFAULT_CAPTURE_TAIL_SECONDS, help="Extra silence appended after playback to finish recording.")
    playrec_parser.add_argument("--monitor-rms", action="store_true", help="Print live RMS levels of the recorded input during playback.")
    playrec_parser.add_argument("--monitor-interval", type=float, default=DEFAULT_MONITOR_INTERVAL_SECONDS, help="RMS reporting interval in seconds.")
    playrec_parser.set_defaults(func=command_playrec)

    analyze_parser = subparsers.add_parser("analyze", help="Analyze a recorded WAV file against an existing reference WAV.")
    analyze_parser.add_argument("capture", help="Recorded WAV file path.")
    analyze_parser.add_argument("--reference", required=True, help="Existing reference WAV file path. Required: the command fails if the file does not exist; no reference is generated automatically.")
    analyze_parser.add_argument("--threshold", type=float, default=DEFAULT_CORRELATION_THRESHOLD, help="Minimum correlation threshold per detected channel.")
    analyze_parser.set_defaults(func=command_analyze)

    validate_parser = subparsers.add_parser("validate", help="Run the full loopback validation from an existing reference WAV: recording and analysis.")
    validate_parser.add_argument("--input-file", "--source", dest="input_file", required=True, help="Existing reference WAV file path. Must be a 16-bit PCM WAV compatible with the selected devices. The command fails if the file does not exist.")
    validate_parser.add_argument("--output-file", "--capture", dest="output_file", required=True, help="Recording WAV file path written by the validation run. The final file is aligned to the reference before analysis.")
    validate_parser.add_argument("--input-device", type=parse_device_argument, required=True, help="Input device name or index. For example: the STM32 USB product string shown by the host.")
    validate_parser.add_argument("--output-device", type=parse_device_argument, required=True, help="Output device name or index. For example: the STM32 USB product string shown by the host.")
    validate_parser.add_argument("--input-channels", type=int, help="Number of input channels to record. If omitted, uses the reference WAV channel count.")
    validate_parser.add_argument("--input-backend", choices=("sounddevice", "sounddevice-wasapi-exclusive", "sounddevice-wdmks", "soundcard", "miniaudio"), default="sounddevice-wdmks", help="Audio backend used for recording. Defaults to sounddevice-wdmks for this validation workflow. sounddevice-wdmks selects the Windows WDM-KS host API through PortAudio; sounddevice-wasapi-exclusive selects exclusive WASAPI. The sounddevice-wasapi-exclusive, sounddevice-wdmks, and miniaudio backends also handle playback.")
    validate_parser.add_argument("--tail-seconds", type=float, default=DEFAULT_CAPTURE_TAIL_SECONDS, help="Extra silence appended after playback to finish recording.")
    validate_parser.add_argument("--monitor-rms", action="store_true", help="Print live RMS levels of the recorded input during playback.")
    validate_parser.add_argument("--monitor-interval", type=float, default=DEFAULT_MONITOR_INTERVAL_SECONDS, help="RMS reporting interval in seconds.")
    validate_parser.add_argument("--threshold", type=float, default=DEFAULT_CORRELATION_THRESHOLD, help="Minimum correlation threshold per detected channel.")
    validate_parser.set_defaults(func=command_validate)

    return parser


def main(argv: Iterable[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(list(argv) if argv is not None else None)
    try:
        return args.func(args)
    except RuntimeError as exc:
        print(f"ERROR: {exc}")
        return 1
    except ValueError as exc:
        print(f"ERROR: {exc}")
        return 1


if __name__ == "__main__":
    raise SystemExit(main())