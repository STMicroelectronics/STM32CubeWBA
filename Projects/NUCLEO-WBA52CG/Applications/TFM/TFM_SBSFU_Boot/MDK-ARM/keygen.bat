set "projectdir=%~dp0"

:: end of updated field
pushd %projectdir%\..\..\TFM_SBSFU_Boot\Src
set tfm_key_dir=%cd%
popd
pushd %projectdir%\..\..\..\..\..\..\Middlewares\Third_Party\mcuboot
set mcuboot_dir=%cd%
popd
set "tfm_keys=%tfm_key_dir%\keys.c"
set "key_eat_priv_bin=%tfm_key_dir%\s_data.bin"

::lines for keys backup
for /f %%# in ('wmic os get localdatetime^|findstr .') do if "%%#" neq "" set date=%%#
set date=%date:~,4%_%date:~4,2%_%date:~6,2%_%date:~8,2%_%date:~10,2%_%date:~12,2%
set "$key_backup_dir=%tfm_key_dir%\%date%_keybcp"
set "cmdcreatedir=mkdir %$key_backup_dir%
%cmdcreatedir%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "cmdcpy=copy %tfm_keys% %$key_backup_dir%"
%cmdcpy%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "cmdcpypem=copy %tfm_key_dir%\*.pem %$key_backup_dir%\"
%cmdcpypem%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "cmdcpysdata=copy %key_eat_priv_bin% %$key_backup_dir%"
%cmdcpysdata%
IF %ERRORLEVEL% NEQ 0 goto :error_key

:start
goto exe:
goto py:
:exe
::line for window executable
set "imgtool=%mcuboot_dir%\scripts\dist\imgtool\imgtool.exe"
set "python="
if exist %imgtool% (
echo Keygen with windows executable
goto Keygen
)
:py
::line for python
echo Keygen with python script
set "imgtool=%mcuboot_dir%\scripts\imgtool\main.py"
set "python=python "

:keygen
set "key_rsa_s=%tfm_key_dir%\root-rsa-2048.pem"
set "command_key=%python%%imgtool% keygen -k %key_rsa_s% -t rsa-2048"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key

set "command_key=%python%%imgtool% getpub -k %key_rsa_s%  > %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key

set "key_rsa_ns=%tfm_key_dir%\root-rsa-2048_1.pem"
set "command_key=%python%%imgtool% keygen -k %key_rsa_ns% -t rsa-2048"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key

set "command_key=%python%%imgtool% getpub -k %key_rsa_ns%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key

@set cnt=0
:rsa_key_construction
:: priv key to encode images
set "key_rsa_enc_priv=%tfm_key_dir%\enc-rsa2048-priv.pem"
set "key_rsa_enc_pub=%tfm_key_dir%\enc-rsa2048-pub.pem"
set "command_key=%python%%imgtool% keygen -k %key_rsa_enc_priv% -t rsa-2048 -e %key_rsa_enc_pub%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool%  getpriv  --minimal -k %key_rsa_enc_priv%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :rsa_key_error

::rsa 3072 auth key
set "key_rsa_2_s=%tfm_key_dir%\root-rsa-3072.pem"
set "command_key=%python%%imgtool% keygen -k %key_rsa_2_s% -t rsa-3072"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpub -k %key_rsa_2_s%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key

set "key_rsa_2_ns=%tfm_key_dir%\root-rsa-3072_1.pem"
set "command_key=%python%%imgtool% keygen -k %key_rsa_2_ns% -t rsa-3072"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpub -k %key_rsa_2_ns%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key

::ecc 256 auth key
set "key_ecc_s=%tfm_key_dir%\root-ec-p256.pem"
set "command_key=%python%%imgtool% keygen -k %key_ecc_s% -t ecdsa-p256"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpub -k %key_ecc_s%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key

::ecc 256 auth key
set "key_ecc_ns=%tfm_key_dir%\root-ec-p256_1.pem"
set "command_key=%python%%imgtool% keygen -k %key_ecc_ns% -t ecdsa-p256"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpub -k %key_ecc_ns%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key

:: priv key to encode images
set "key_ecc_enc_priv=%tfm_key_dir%\enc-ec256-priv.pem"
set "key_ecc_enc_pub=%tfm_key_dir%\enc-ec256-pub.pem"
set "command_key=%python%%imgtool% keygen -k %key_ecc_enc_priv% -t ecdsa-p256 -e %key_ecc_enc_pub%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpriv  --minimal -k %key_ecc_enc_priv%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
::eat key
set "key_eat=%tfm_key_dir%\tfm_initial_attestation_key.pem"
set "command_key=%python%%imgtool% keygen -k %key_eat% -t ecdsa-p256-ssl"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpriv --minimal -k %key_eat%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getprivbin --minimal -k %key_eat%  > %key_eat_priv_bin%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
::huk key
set "key_huk=%tfm_key_dir%\huk.bin"
set "command_key=%python%%imgtool% keygen -k %key_huk% -t sym32"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpriv -k %key_huk%  >> %tfm_keys%"
%command_key%
IF %ERRORLEVEL% NEQ 0 goto :error_key
exit 0

:error_key
echo "%command_key% : failed" >> %projectdir%\\output.txt
exit 1

:rsa_key_error
@set /a "cnt=%cnt%+1"
IF %cnt% GEQ 2 (goto :error_key) else (goto :rsa_key_construction)
