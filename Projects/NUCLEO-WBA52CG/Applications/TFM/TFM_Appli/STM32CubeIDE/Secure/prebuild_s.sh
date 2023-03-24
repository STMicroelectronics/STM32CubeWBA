#!/bin/bash -

# Absolute path to this script
SCRIPT=$(readlink -f $0)
# Absolute path this script
projectdir=`dirname $SCRIPT`

Third_Party=$projectdir"/../../../../../../../Middlewares/Third_Party"
TFM_TEST_PATH=$Third_Party"/trustedfirmware/test"
TFM_PATH=$Third_Party"/trustedfirmware"
TOOLS=$Third_Party"/trustedfirmware/tools"
# Recopy config for generation
CONFIG=$projectdir"/../../Secure/Src"
PARTITION=$Third_Party"/trustedfirmware/secure_fw/partitions"

# Config from application
cp $CONFIG"/tfm_manifest_list.yaml" $projectdir
cp $CONFIG"/tfm_generated_file_list.yaml" $projectdir

# Config platform customizeable
cp $CONFIG"/tfm_app_rot.yaml" $projectdir
cp $CONFIG"/tfm_platform.yaml" $projectdir

# Config from middleware standard services
cp $PARTITION"/crypto/tfm_crypto.yaml" $projectdir
cp $PARTITION"/protected_storage/tfm_protected_storage.yaml" $projectdir
cp $PARTITION"/internal_trusted_storage/tfm_internal_trusted_storage.yaml" $projectdir
cp $PARTITION"/initial_attestation/tfm_initial_attestation.yaml" $projectdir
cp $PARTITION"/firmware_update/tfm_firmware_update.yaml" $projectdir
cp $PARTITION"/tfm_ffm11_partition/tfm_ffm11_partition.yaml" $projectdir
# Copy template file from middleware
cp $PARTITION"/manifestfilename.template" $projectdir
cp $PARTITION"/partition_intermedia.template" $projectdir
cp $PARTITION"/tfm_service_list.inc.template" $projectdir
cp $TFM_PATH"/interface/include/tfm_veneers.h.template" $projectdir
cp $TFM_PATH"/secure_fw/spm/cmsis_psa/tfm_secure_irq_handlers_ipc.inc.template" $projectdir
cp $TFM_PATH"/secure_fw/spm/cmsis_psa/tfm_spm_db_ipc.inc.template" $projectdir
cp $TFM_PATH"/interface/include/psa_manifest/sid.h.template" $projectdir
cp $TFM_PATH"/interface/include/psa_manifest/pid.h.template" $projectdir
# Copy config for test
cp $TFM_TEST_PATH"/test_services/tfm_core_test/tfm_ss_core_test.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_core_test_2/tfm_ss_core_test_2.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_secure_client_service/tfm_secure_client_service.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_ipc_service/tfm_ipc_service_test.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_secure_client_service/tfm_secure_client_service.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_ipc_client/tfm_ipc_client_test.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_ps_test_service/tfm_ps_test_service.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_ipc_service/tfm_ipc_service_test.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_ipc_client/tfm_ipc_client_test.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_irq_test_service_1/tfm_irq_test_service_1.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_ps_test_service/tfm_ps_test_service.yaml" $projectdir
cp $TFM_TEST_PATH"/test_services/tfm_secure_client_2/tfm_secure_client_2.yaml" $projectdir
#cp $TFM_TEST_PATH"/test_services/tfm_core_test/tfm_core_test" $projectdir
# Config platform customizeable
cp $CONFIG"/tfm_app_rot.yaml" $projectdir
cp $CONFIG"/tfm_platform.yaml" $projectdir

export projectdir

tfm_parse_manifest_list=$TOOLS"/dist/tfm_parse_manifest_list/tfm_parse_manifest_list.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$tfm_parse_manifest_list" ]; then
command=$tfm_parse_manifest_list" -o "$projectdir"/g -m "$projectdir"/tfm_manifest_list.yaml -f "$projectdir"/tfm_generated_file_list.yaml"
else
command=python" "$TOOLS"/dist/pyscript/tfm_parse_manifest_list.py -o "$projectdir"/g -m "$projectdir"/tfm_manifest_list.yaml -f "$projectdir"/tfm_generated_file_list.yaml"
fi
$command >> $projectdir"/output.txt"
command="arm-none-eabi-gcc -E -P -xc -DBL2 -D"$2" -DTFM_PSA_API  -I../../../../Linker -o./output.ld  ../tfm_common_s.ld"
$command >> $projectdir"/output.txt"