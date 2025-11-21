import pytest
from click.testing import CliRunner
import os
import filecmp 
from ACutils import *
from main import *
import os.path
import random
from sys import stderr
import shutil

# Test Applicfg utils classes
# run from test folder with : pytest -s AppliCfg_test_utils.py

def workpath(filename:str)->str:
    #directory=os.path.join(os.path.abspath('.'), '__INTERNAL_TESTS__/')
    directory=os.path.abspath('.')
    return os.path.join(directory, filename)

def text_diff(file1, file2):
    with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
        index = 0
        content1 = f1.read().replace(b'\r\n', b'\n')
        content2 = f2.read().replace(b'\r\n', b'\n')
        res=False
        if len(content1)>len(content2):
            minlen=len(content2)
            print(f"{file1} larger")
        elif len(content1)<len(content2):
            minlen=len(content1)
            print(f"{file2} larger")
        else:
            minlen=len(content1)
            res=True
        while index<minlen:
            octet1 = content1[index]
            octet2 = content2[index]
            index+=1
            if octet1 != octet2:
                # Octets différents trouvés
                print(f"Difference index {index}: {octet1} != {octet2}")
                res=False
                break
        return res

def delfile(filename : str)->None:
    if os.path.exists(filename):
        os.remove(filename)

def test_LOG(capfd):
    log1=LOG(False,True)
    log2=LOG(False,False)#no timestamp
    log3=LOG(True,False)#quiet option
    log1.info("test")
    captured = capfd.readouterr()
    assert "test" in captured.out and "[INF]" in captured.out
    lena=len(captured.out)
    log2.info("test")
    captured= capfd.readouterr()
    assert "test" in captured.out
    assert lena>len(captured.out)
    arg=["param1", "param2"]
    log2.array(arg)
    captured = capfd.readouterr()
    lenb=len(captured)
    assert "param1" in captured.out and "param2" in captured.out
    log2.begin("test")
    captured = capfd.readouterr()
    assert captured.out[:14]=="[INF] ########"
    log3.info("test")
    captured = capfd.readouterr()
    assert "" ==captured.out
    try:
        log2.error("test")
        assert False
    except SystemExit as e:
        captured = capfd.readouterr()
        assert "test" in str(e) and "[ERR]" in str(e)


def test_MANAGE_FILE():
    delfile("nofile")
    delfile("testfile")
    with open("testfile", 'w') as f:
        f.write("data")
    f.close()
    log=LOG(False,True)
    #check file verification
    mf=MANAGE_FILE("testfile",log, True)
    assert mf.name()=="testfile"
    #no file verification, get the filename
    mf=MANAGE_FILE("testfile2\\file.temp",log, False)
    assert mf.name()=="file.temp"
    try:
        mf=MANAGE_FILE("nofile",log)
        assert False
    except:
        assert True
    delfile("testfile")

def test_StringValue():
    x=StringValue(LOG(False,True))
    x.set("0xc125a")
    assert x.is_none()==False
    assert int(x)==0xc125a
    assert x.getHex(None, False)=="0xc125a"
    assert x.getHex(None,True)=="0xC125A"
    assert x.getHex(6, False)=="0x0c125a"
    x.set()
    assert x.is_none()!=False
    x.set("")
    assert x.is_none()==True
    x.set(1254)
    assert int(x)==1254
    assert x.getInt(5)=="01254"
    x.set(0xFF0000F)
    x.modify_hex_value(0xA5,8)
    assert x.getHex(None,True)=="0xFF0A50F"
    x.set("")
    x.set(12.54)
    assert x.is_none()==False

@pytest.mark.parametrize("filename, data, script", [("temp.sh","toto=124", "shell"), ("temp.bat","set toto=124", "batch"), ("temp.h","#define toto 124", "c")])
def test_SCRIPT_APPLI_1(filename,data, script):
    delfile(filename)
    with open(filename, 'w') as f:
        f.write("#fill comment"+"\n")
        f.write(data+"\n")
        f.write("#fill comment"+"\n")
        f.write("#false positif toto comment"+"\n")
    f.close()
    script_file=SCRIPT_APPLI(filename,LOG(False,True))
    if script.lower()=="shell":
        assert script_file.is_shell()==True
        assert script_file.pattern("toto","=", False)==r"^\s*toto\s*=\s*"
        rpattern=r"^\s*toto\s*=\s*"
    elif script.lower()=="batch":
        assert script_file.is_batch()==True
        assert script_file.pattern("toto", "=", False)==r"^\s*set\s+toto\s*=\s*"
        rpattern=r"^\s*set\s*toto\s*=\s*"
    elif script.lower()=="c":
        assert script_file.is_header()==True
        assert script_file.pattern("toto", "", False)==r"^\s*#define\s+toto\s+"
        rpattern=r"^\s*#define\s+toto\s+"
    else:
        assert False
    script_file.read()
    assert script_file.get_file_value(rpattern)=="124"
    assert script_file.modify_file_value(rpattern, 124, 789)==True
    assert script_file.get_file_value(rpattern)=="789"
    assert script_file.modify_file_value(rpattern, 789, "0x25")==True
    assert script_file.get_file_value(rpattern)=="0x25"
    assert script_file.modify_file_value(rpattern, 465, 456)==False
    assert script_file.modify_file_value(rpattern, None, 456)==True
    assert script_file.get_file_value(rpattern)=="456"
    script_file.save()
    delfile(filename)

def test_SCRIPT_APPLI_2():
    filename="temp.c"
    delfile(filename)
    with open(filename, 'w') as f:
        f.write("#define toto 1\n")
        f.write("#define titi 2\n")
        f.write("#define tutu 3\n")
        f.write("/* #define tata 4 */\n")
    f.close()
    script_file=SCRIPT_APPLI(filename,LOG(False,True))
    assert script_file.toggle_comment_line("titi","comment")==True
    script_file.logs()
    assert script_file.toggle_comment_line("tata","uncomment")==True
    assert script_file.toggle_comment_line("toto","comment")==True
    assert script_file.toggle_comment_line("tata", "uncomment")==False
    assert script_file.toggle_comment_line("titi", "comment")==False
    assert script_file.toggle_comment_line("tutu", "uncomment")==False
    assert script_file.toggle_comment_line("bidule","comment")==False
    script_file2=SCRIPT_APPLI(filename,LOG(False,True))
    assert script_file.toggle_comment_line("titi","comment")==False
    delfile(filename)

@pytest.mark.parametrize("filename, macro, value", [
                        ("input_tests/macros_real_1.c", "RE_IMAGE_FLASH_DATA_SECURE_UPDATE", 0x8170000), \
                        ("input_tests/macros_real_1.c", "RE_CRYPTO_SCHEME", 0x2), \
                        ("input_tests/macros_real_1.c", "RE_BL2_WRP_START", 0x10000), \
                        ("inc_tests/simple_test_real.h","FLASH_NS_PARTITION_SIZE", 0xa0000), \
                        ("linker_tests/H5_linker_real.icf","CODE_SIZE", 0x20000)])
def test_SCRIPT_APPLI_3(filename, macro, value):
    script_file=SCRIPT_APPLI(filename,LOG(False,True))
    assert hex(script_file.get_macro_value(macro))==hex(value)

def test_BIN_FILE():
    filename='test.bin'
    delfile(filename)
    with open(filename, 'wb') as f:
        f.write(bytearray([1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf]))
        f.close()
    bin_file=BIN_FILE(filename,LOG(False,True))
    bin_file.read()
    assert bin_file.modify_bin_content(5,0xFF)==True
    assert bin_file.modify_bin_content(100,0xFF)==False
    assert len(bin_file)==15
    bin_file.save()
    sha256=BIN_FILE('sha256.bin',LOG(False,True))
    sha256.compute_sha256(bin_file)
    sha256.compare(bytearray([0x1a,0xbd,0xaf,0x08,0x53,0xae,0x07,0xa3,0x0c,0xcb,0x88,0xf4,0x0f,0xb3,0x08,0x7e,0x63,0x44,0x6c,0x03,0x10,0x48,0xda,0xac,64,0xfc,0xf6,0x5c,0xe1,0x41,0xa0,0x84]))
    sha256.save()
    delfile('sha256.bin')
    delfile(filename)


def test_definevalue():
    runner = CliRunner()
    #py ../AppliCfg.py definevalue -xml "xml_tests\OEMiRoT_Config.xml" -nxml "Nb Secure Modules" -n NB_MODULE "inc_tests\fwu.h" --vb
    shutil.copy("inc_tests/fwu.h", "temp.h")
    result = runner.invoke(definevalue, ["--xml=xml_tests/SM_Config_General.xml", "--xml_name=Nb Secure Modules", '--name=NB_MODULE', "--vb", "temp.h"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.h", "inc_tests/fwu.res", shallow=False)==True
    shutil.copy("inc_tests/ukapp_config_sections.h", "temp.h")
    #py ../AppliCfg.py definevalue -xml xml_tests/SM_Config_General.xml  -nxml "S Module 0 primary offset" -nxml "S Module 0 size" -n UKAPP_MEMORY_REGION_CODE_ORIGIN_END -e "0x0C000000 + val1*0x2000 + val2*0x2000 - 0x800 + 0x20" temp.h --vb
    result = runner.invoke(definevalue, ["--xml=xml_tests/SM_Config_General.xml", "--xml_name=S Module 0 primary offset", "--xml_name=S Module 0 size", '--name=UKAPP_MEMORY_REGION_CODE_ORIGIN_END',"--expression=0x0C000000 + val1*0x2000 + val2*0x2000 - 0x800 + 0x20", "--vb","temp.h"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.h", "inc_tests/ukapp_config_sections.res", shallow=False)==True
    #py ../AppliCfg.py definevalue --layout linker_tests/image_macros_preprocessed_bl2.c RE_IMAGE_PRIMARY_AREA_OFFSET -n VECT_TAB_OFFSET inc_tests/system_stm32u0xx.c --vb
    shutil.copy("inc_tests/system_stm32u0xx.c", "temp.h")
    result = runner.invoke(definevalue, ["--layout=inc_tests/image_macros_preprocessed_bl2.c", '--name=VECT_TAB_OFFSET', "--macro=RE_IMAGE_PRIMARY_AREA_OFFSET", "--vb","temp.h"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.h", "inc_tests/system_stm32u0xx.res", shallow=False)==True
    shutil.copy("linker_tests/stm32h573xx_s.sct", "temp.sct")
    #py ../AppliCfg.py definevalue --layout linker_tests/image_macros_preprocessed_bl2.4.c -m RE_AREA_0_OFFSET -n S_CODE_OFFSET  --vb temp.sct
    result = runner.invoke(definevalue, ["--layout=inc_tests/image_macros_preprocessed_bl2.4.c", '--name=S_CODE_OFFSET', "--macro=RE_AREA_0_OFFSET", "--vb","temp.sct"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.sct", "linker_tests/stm32h573xx_s.res", shallow=False)==True
    #py ../AppliCfg.py definevalue --name=FLASH_AREA_SCRATCH_SIZE  --value=0x54 --parenthesis --occurrence=2 --vb temp.h
    shutil.copy("inc_tests/flash_layout.h", "temp.h")
    result = runner.invoke(definevalue, ['--name=FLASH_AREA_SCRATCH_SIZE', "--parenthesis", "--value=0x54", "--occurrence=2", "--vb","temp.h"])
    print(result.output)
    assert result.exit_code == 0
    #shutil.copy("temp.h", "inc_tests/flash_layout.3.res")
    assert filecmp.cmp("temp.h", "inc_tests/flash_layout.3.res", shallow=False)==True

def test_hashcontent():
    runner = CliRunner()
    #py ../AppliCfg.py hashcontent -i input_tests/user_password.bin  -d input_tests/password.bin -h 0x80000000 --create --vb temp.bin
    result = runner.invoke(hashcontent, ["--input_data=input_tests/user_password.bin", "--da_bin_file=input_tests/password.bin", "--header=0x80000000", "temp.bin", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bin", "input_tests/password.res", shallow=False)==True
    #py ../AppliCfg.py hashcontent -i input_tests/user_password.bin --vb temp.bin
    result = runner.invoke(hashcontent, ["--input_data=input_tests/user_password.bin", "temp.bin", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    #py ../AppliCfg.py hashcontent -i input_tests/user_password.bin --vb sha.bin --password --da_bin_file=test.bin -h 0x000000800c000000
    result = runner.invoke(hashcontent, ["--input_data=input_tests/user_password.bin", "test.bin", "--vb", "--password", "--da_bin_file=da.bin", "--header=0x000000800c000000", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("da.bin", "input_tests/da.bin", shallow=False)==True
    #py ../AppliCfg.py hashcontent sha.bin --input_data=xml_tests/SM_Config_General.xml --da_bin_file="composed.bin" --tail="AABCEF14C0145" --vb
    result = runner.invoke(hashcontent, ["--input_data=xml_tests/SM_Config_General.xml", "--da_bin_file=composed.bin", "--tail=AABCEF14C0145", "sha.bin", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    #shutil.copy("sha.bin", "inc_tests/sha.bin")
    assert filecmp.cmp("sha.bin", "inc_tests/sha.bin", shallow=False)==True
    
def test_linker():
    #py ../AppliCfg.py linker -xml xml_tests/SM_Config_General.xml -nxml "Secure SRAM End address" -n RAM_S_END -e "val1-0x10000000" linker_tests/stm32h573xx_flash.icf
    shutil.copy("linker_tests/stm32h573xx_flash.1.icf", "temp.icf")
    runner = CliRunner()
    result = runner.invoke(linker, ["--xml=xml_tests/SM_Config_General.xml", "--xml_name=NS appli size", "--name=CODE_SIZE", "--vb", "temp.icf"])
    print(result.output)
    assert result.exit_code == 0
    result = runner.invoke(linker, ["--xml=xml_tests/SM_Config_General.xml", "--xml_name=Secure SRAM End address", "--name=RAM_S_END", "--expression=val1-0x10000000", "--vb", "temp.icf"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.icf", "linker_tests/stm32h573xx_flash.1.res", shallow=False)==True
    shutil.copy("linker_tests/stm32h7s7xx_flash.2.icf", "temp.icf")
    #py ../AppliCfg.py linker --layout=linker_tests/image_macros_preprocessed_bl2.1.c -m RE_PARTITION_START -n PARTITION_START --vb linker_tests/stm32h7s7xx_flash.2.icf
    result = runner.invoke(linker, ["--layout=linker_tests/image_macros_preprocessed_bl2.1.c", "--macro=RE_PARTITION_START", "--name=PARTITION_START", "--vb", "temp.icf"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.icf", "linker_tests/stm32h7s7xx_flash.2.res", shallow=False)==True
    #py ../AppliCfg.py linker --layout=linker_tests/tmp.c -m DATA_SIZE -n FLASH_DATA_AREA_SIZE --vb temp.sct
    assert filecmp.cmp("temp.icf", "linker_tests/stm32h7s7xx_flash.2.res", shallow=False)==True
    shutil.copy("linker_tests/stm32u0xx_app.sct", "temp.sct")
    result = runner.invoke(linker, ["--layout=linker_tests/tmp.c", "--macro=DATA_SIZE", "--name=FLASH_DATA_AREA_SIZE", "--vb", "temp.sct"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.sct", "linker_tests/stm32u0xx_app.res", shallow=False)==True
    
def test_setdefine():
    #py ../AppliCfg.py setdefine --vb -a comment -n OEMUROT_ENABLE inc_tests/flash_layout.h
    runner = CliRunner()
    shutil.copy("inc_tests/flash_layout.h", "temp.h")
    result = runner.invoke(setdefine, ["--name=OEMUROT_ENABLE", "--action=comment", "--vb", "temp.h"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.h", "inc_tests/flash_layout.1.res", shallow=False)==True
    result = runner.invoke(setdefine, ["--name=OEMUROT_ENABLE", "--action=comment", "--vb", "temp.h"]) #2 fois, pas d'erreur
    assert result.exit_code == 0
    result = runner.invoke(setdefine, ["--name=OEMUROT_ENABLE", "--action=uncomment", "--vb", "temp.h"])
    print(result.output)
    assert result.exit_code == 0
    #assert filecmp.cmp("temp.h", "inc_tests/flash_layout.h", shallow=False)==True #not an iso operation
    

def test_xmlval():
    #py ../AppliCfg.py xmlval -xml xml_tests/SM_Config_General.xml -nxml "NS appli size" -n "Firmware area Size" -e "val1*0x2000" xml_tests/SM_Code_Image.1.xml
    runner = CliRunner()
    shutil.copy("xml_tests/SM_Code_Image.1.xml", "temp.xml")
    result = runner.invoke(xmlval, ["--xml=xml_tests/SM_Config_General.xml", "--xml_name=NS appli size", "--name=Firmware area Size", "--expression=val1*0x1850", "--vb", "temp.xml"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.xml", "xml_tests/SM_Code_Image.1.res", shallow=False)==True
    shutil.copy("xml_tests/SM_Config_General.xml", "temp.xml")
    #py ../AppliCfg.py xmlval -b "C:/git/sec-manager_h5/Firmware/Projects/STM32H573I-DK/ROT_Provisioning/SM/Binary/FlashLayoutConfigs.bin" -b_el_idx 30 -nxml_el_idx "Flash Layout configuration index" -n "NS appli size" -nxml "NS reserved area size" -e "val1-(val2/2)" -cond 'val2>0 and (val2 %% 2) == 0' "temp.xml" --vb
    result = runner.invoke(xmlval, ["--binary_file=xml_tests/FlashLayoutConfigs.bin", "temp.xml", "--binary_element_index=30", "--xml_name=NS reserved area size", "--name=NS appli size", "--expression=val1-(val2/2)", "--condition=val2>0 and (val2 % 2) == 0", "--xml_name_layout_index=Flash Layout configuration index", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.xml", "xml_tests/SM_Code_Image.2.res", shallow=False)==True
    shutil.copy("xml_tests/OEMiROT_Code_Image.xml", "temp.xml")
    #py ../AppliCfg.py xmlval --vb -v mypath/Binary/rot_app.bin --string -n "Firmware binary input file" xml_tests/OEMiROT_Code_Image.xml
    result = runner.invoke(xmlval, [f"--value=mypath/Binary/rot_app.bin", "--string", "--name=Firmware binary input file", "temp.xml", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.xml", "xml_tests/OEMiROT_Code_Image.1.res", shallow=False)==True
    #py ../AppliCfg.py xmlval  --name="Authentication non secure key" --txml_tag="Default" xml_tests/OEMiROT_NS_Code_Image.xml -vb "../Keys/mykey"
    shutil.copy("xml_tests/OEMiROT_NS_Code_Image.xml", "temp.xml")
    result = runner.invoke(xmlval, ["--name=Authentication non secure key", "temp.xml", "--xml_tag=Default", "--value=../Keys/mykey", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    result = runner.invoke(xmlval, ["--name=Authentication non secure key", "temp.xml", "--value=../Keys/mykey", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.xml", "xml_tests/OEMiROT_NS_Code_Image.res", shallow=False)==True
    #py ../AppliCfg.py xmlval --layout=linker_tests/stm32h7sxx_iloader.sct --macro=CODE_OFFSET --name="STiROT_iLoader offset" temp.xml --vb
    shutil.copy("linker_tests/STiROT_Config.xml", "temp.xml")
    result = runner.invoke(xmlval, ["--layout=linker_tests/stm32h7sxx_iloader.sct", "--macro=CODE_OFFSET", "--name=STiROT_iLoader offset", "--vb", "temp.xml"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.xml", "linker_tests/STiROT_Config.res", shallow=False)==True
    #py ../AppliCfg.py xmlval --layout=xml_tests/image_macros_preprocessed_bl2.c --macro=RE_IMAGE_FLASH_NON_SECURE_UPDATE --second_macro=RE_IMAGE_FLASH_SECURE_UPDATE --command=x --vb temp.xml
    shutil.copy("xml_tests/OEMiROT_Code_Image.xml", "temp.xml")
    result = runner.invoke(xmlval, ["--layout=xml_tests/image_macros_preprocessed_bl2.c", "--macro=RE_IMAGE_FLASH_NON_SECURE_UPDATE", "--second_macro=RE_IMAGE_FLASH_SECURE_UPDATE","--command=x", "--value=0","--vb", "temp.xml"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.xml", "xml_tests/OEMiROT_Code_Image.2.res", shallow=False)==True
    #py ../AppliCfg.py xmlval  --macro RE_ENCRYPTION --name="Encryption key" --xml_tag="Enable" OEMiROT_S_Code_Image.xml -vb
    
    

def test_setob():
    #py ../Applicfg.py setob --layout=linker_tests/image_macros_preprocessed_bl2.1.c -b wrps -ms RE_BL2_WRP_OFFSET -me RE_BL2_WRP_SIZE -msec RE_BL2_WRP_SIZE -d 0x2000 --protected_area 0x10000 --vb linker_tests/ob_flash_programming.1.bat
    shutil.copy("linker_tests/ob_flash_programming.1.bat", "temp.bat")
    runner = CliRunner()
    result = runner.invoke(setob, ["--layout=linker_tests/image_macros_preprocessed_bl2.1.c", "--begin=wrps", "--macro_start=RE_BL2_WRP_OFFSET", "--macro_end=RE_BL2_WRP_SIZE", "--macro_sectors=RE_BL2_WRP_SIZE", "--division=0x2000", "--protected_area=0x10000", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    #py ../AppliCfg.py setob --layout linker_tests/image_macros_preprocessed_bl2.1.c -b hdp_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_HDP_BLOCK_NBR -d 0x100 linker_tests/ob_flash_programming.1.bat --vb
    result = runner.invoke(setob, ["--layout=linker_tests/image_macros_preprocessed_bl2.1.c", "--begin=hdp_start", "--macro_start=RE_BL2_HDP_START", "--macro_end=RE_BL2_WRP_SIZE", "--macro_sectors=RE_HDP_BLOCK_NBR", "--division=0x2000", "--protected_area=0x100", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "linker_tests/ob_flash_programming.1.res", shallow=False)==True
    #py ../AppliCfg.py setob --layout linker_tests/image_macros_preprocessed_bl2.3.c -b wrpgrp1 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x8000 ob_flash_programming.3.bat --vb
    shutil.copy("linker_tests/ob_flash_programming.3.bat", "temp.bat")
    result = runner.invoke(setob, ["--layout=linker_tests/image_macros_preprocessed_bl2.3.c", "--begin=wrpgrp1", "--macro_start=RE_BL2_WRP_START", "--macro_end=RE_BL2_WRP_END", "--macro_sectors=RE_FLASH_PAGE_NBR", "--division=0x8000", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "linker_tests/ob_flash_programming.3.res", shallow=False)==True
    #py ../AppliCfg.py setob --layout stirot_tests/stm32h7s7xx_flash.icf --begin=OEMiLoader_address --macro_start=CODE_OFFSET --macro_end CODE_SIZE --macro_sectors=CODE_SIZE --division=0x2000 --vb ob_flash_programming.bat
    shutil.copy("stirot_tests/ob_flash_programming.bat", "temp.bat")
    result = runner.invoke(setob, ["--layout=stirot_tests/stm32h7s7xx_flash.icf", "--begin=OEMiLoader_address", "--macro_start=CODE_OFFSET", "--macro_end=CODE_SIZE", "--macro_sectors=CODE_SIZE", "--division=0x2000", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "stirot_tests/ob_flash_programming.2.res", shallow=False)==True


def test_flash():
    shutil.copy("linker_tests/ob_flash_programming.2.bat", "temp.bat")
    runner = CliRunner()
    #py ../AppliCfg.py flash --layout linker_tests/image_macros_preprocessed_bl2.2.c -b wrp1b_end -m RE_LOADER_WRP_END -d 0x800 ob_flash_programming.2.bat --vb
    result = runner.invoke(flash, ["--layout=linker_tests/image_macros_preprocessed_bl2.2.c", "--begin=wrp1b_end", "--macro=RE_LOADER_WRP_END", "--division=0x800", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    #update empty value in file
    #py ../AppliCfg.py flash --layout linker_tests/image_macros_preprocessed_bl2.2.c -b wrp1b_end -m RE_LOADER_WRP_END -d 0x800 ob_flash_programming.2.bat --vb
    result = runner.invoke(flash, ["--layout=linker_tests/image_macros_preprocessed_bl2.2.c", "--begin=hdp_end", "--macro=RE_LOADER_WRP_END", "--division=0x800", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "linker_tests/ob_flash_programming.2.res", shallow=False)==True
    
    #py ../AppliCfg.py flash -xml xml_tests/SM_Code_Image.1.xml -n "Rom fixed" -e "(val1+0x400)" -b ns_code_image tmp.bat --vb
    shutil.copy("xml_tests/download.bat", "temp.bat")
    result = runner.invoke(flash, ["--xml=xml_tests/SM_Code_Image.1.xml", "--name=Rom fixed", "--expression=(val1+0x400)", "--begin=ns_code_image", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    #shutil.copy("temp.bat","xml_tests/download.res")
    assert filecmp.cmp("temp.bat", "xml_tests/download.res", shallow=False)==True
    shutil.copy("inc_tests/flash_CFG_0.bat", "temp.bat")
    #py ../AppliCfg.py flash -xml inc_tests/HDPL2_Config_CFG_0.xml -n ITS_offset -n ITS_size -b secwm2_end -e "(val1 + val2) -cons1"  -cons 129 flash_CFG_0.bat
    result = runner.invoke(flash, ["--xml=inc_tests/HDPL2_Config_CFG_0.xml", "--name=ITS_offset", "--name=ITS_size", "--expression=(val1 + val2) -cons1", "--begin=secwm2_end", "--constants=129", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    #shutil.copy("temp.bat", "inc_tests/flash_CFG_0.res")
    assert filecmp.cmp("temp.bat", "inc_tests/flash_CFG_0.res", shallow=False)==True
    
def test_modify():
    #py ../AppliCfg.py modifyfilevalue --variable bootPath --delimiter = --value OEMiROT map.properties --str --vb
    shutil.copy("linker_tests/map.properties", "temp.properties")
    runner = CliRunner()
    result = runner.invoke(modifyfilevalue, ["--variable=bootPath", "--delimiter==", "--value=STiROT", "temp.properties", "--str", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert text_diff("temp.properties", "linker_tests/map.res")==True
    shutil.copy("inc_tests/simple_test.h", "temp.h")
    #py ../AppliCfg.py modifyfilevalue --variable OEMISB_OB_RDP_LEVEL_VALUE --value OB_RDP_LEVEL_1 temp.h --str --vb
    result = runner.invoke(modifyfilevalue, ["--variable=OEMISB_OB_RDP_LEVEL_VALUE", "--value=OB_RDP_LEVEL_1", "temp.h", "--str", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.h", "inc_tests/simple_test.res", shallow=False)==True
    shutil.copy("inc_tests/flash_layout.h", "temp.h")
    result = runner.invoke(modifyfilevalue, ["--variable=FLASH_PARTITION_SIZE","--value=(2*FLASH_AREA_IMAGE_SECTOR_SIZE)","--str","--vb", "temp.h"])    
    print(result.output)
    assert result.exit_code == 0
    result = runner.invoke(modifyfilevalue, ["--variable=FLASH_MAX_APP_PARTITION_SIZE","--value=(0x2000+FLASH_PARTITION_SIZE)","--str","--vb", "temp.h"])    
    #shutil.copy("temp.h", "inc_tests/flash_layout.test.res")
    assert filecmp.cmp("temp.h", "inc_tests/flash_layout.2.res", shallow=False)==True
    #py ../AppliCfg.py modifyfilevalue --variable=hdp_end --value="0x1" input_tests/provisioning.sh --vb --str --delimiter="="
    shutil.copy("input_tests/provisioning.sh", "temp.sh")
    result = runner.invoke(modifyfilevalue, ['--variable=hdp_end', "--delimiter==", "--value=0x55", "--str", "--vb","temp.sh"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.sh", "input_tests/provisioning.res", shallow=False)==True
    #py ../AppliCfg.py modifyfilevalue --variable key --delimiter = --value Keys/key_1_root.pem --str --vb temp.bat
    shutil.copy("input_tests/regression.bat", "temp.bat")
    result = runner.invoke(modifyfilevalue, ['--variable=key', "--delimiter==", "--value=Keys/key_1_root.pem", "--str", "--vb","temp.bat"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "input_tests/regression.res", shallow=False)==True

def test_obscript():
    runner = CliRunner()
    #py ../AppliCfg.py obscript --xml stirot_tests/STiROT_Config.xml -ob ECC_ON_SRAM -sp "SRAM ECC management activation" --vb ob_flash_programming.bat
    shutil.copy("stirot_tests/ob_flash_programming.bat", "temp.bat")
    result = runner.invoke(obscript, ["--xml=stirot_tests/STiROT_Config.xml", "--option_byte=ECC_ON_SRAM", "--secure_pattern=SRAM ECC management activation", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "stirot_tests/ob_flash_programming.1.res", shallow=False)==True
    #py ../AppliCfg.py obscript --xml stirot_tests/STiROT_Config.H5.0.xml -ob SECWM1_STRT --secure_pattern="Size of the secure area inside the firmware execution area" --division=0x2000 --code_size="Firmware area size" --bank_size=0x100000 --code_offset="Firmware execution area offset" "--full_sec=Is the firmware full secure" --vb temp.bat
    shutil.copy("stirot_tests/H5_programming_real.bat", "temp.bat")
    result = runner.invoke(obscript, ["--xml=stirot_tests/STiROT_Config.H5.0.xml", "--option_byte=SECWM1_STRT", "--division=0x2000", "--secure_pattern=Size of the secure area inside the firmware execution area",  "--code_size=Firmware area size", "--bank_size=0x100000", "--code_offset=Firmware execution area offset", "--full_sec=Is the firmware full secure","temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "stirot_tests/ob_flash_programming.4.res", shallow=False)==True
    #py ../AppliCfg.py obscript --xml stirot_tests/STiROT_Config.H5.1.xml -ob SECWM1_STRT --secure_pattern="Size of the secure area inside the firmware execution area" --division=0x2000 --code_size="Firmware area size" --bank_size=0x100000 --code_offset="Firmware execution area offset" "--full_sec=Is the firmware full secure" --vb ob_flash_programming.bat
    shutil.copy("stirot_tests/H5_programming_real.bat", "temp.bat")
    result = runner.invoke(obscript, ["--xml=stirot_tests/STiROT_Config.H5.1.xml", "--option_byte=SECWM1_STRT", "--division=0x2000", "--secure_pattern=Size of the secure area inside the firmware execution area",  "--code_size=Firmware area size", "--bank_size=0x100000", "--code_offset=Firmware execution area offset", "--full_sec=Is the firmware full secure","temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "stirot_tests/ob_flash_programming.5.res", shallow=False)==True

    #With no parameter, use the xml data
    #py ../AppliCfg.py obscript --xml stirot_tests/STiROT_Config.xml -ob SECWM --secure_pattern="Size of the secure area inside the firmware execution area" --code_size="Firmware area size" --code_offset="Firmware execution area offset" "--full_sec=Is the firmware full secure" --vb temp.bat
    shutil.copy("stirot_tests/H5_programming_real.bat", "temp.bat")
    result = runner.invoke(obscript, ["--xml=stirot_tests/STiROT_Config.H5.1.xml", "--option_byte=SECWM1_STRT", "--secure_pattern=Size of the secure area inside the firmware execution area",  "--code_size=Firmware area size", "--code_offset=Firmware execution area offset", "--full_sec=Is the firmware full secure","temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "stirot_tests/ob_flash_programming.6.res", shallow=False)==True
    #py ../AppliCfg.py obscript --xml stirot_tests/STiROT_Config.H5.2.xml -ob SECWM1_STRT --secure_pattern="Size of the secure area inside the firmware execution area" --division=0x2000 --code_size="Firmware area size" --bank_size=0x100000 --code_offset="Firmware execution area offset" "--full_sec=Is the firmware full secure" --vb ob_flash_programming.sh
    shutil.copy("stirot_tests/ob_flash_programming.1.sh", "temp.sh")
    result = runner.invoke(obscript, ["--xml=stirot_tests/STiROT_Config.H5.2.xml", "--option_byte=SECWM1_STRT", "--secure_pattern=Size of the secure area inside the firmware execution area",  "--code_size=Firmware area size", "--code_offset=Firmware execution area offset", "--full_sec=Is the firmware full secure","temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.sh", "stirot_tests/ob_flash_programming.7.res", shallow=False)==True
    #py ../AppliCfg.py obscript --xml stirot_tests/STiROT_Config.H7RS.xml -ob "ECC_ON_SRAM" -sp "secure_pattern=SRAM ECC management activation" --vb temp.sh
    shutil.copy("stirot_tests/ob_flash_programming.2.sh", "temp.sh")
    result = runner.invoke(obscript, ["--xml=stirot_tests/STiROT_Config.xml", "--option_byte=ECC_ON_SRAM", "--secure_pattern=SRAM ECC management activation", "temp.sh", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.sh", "stirot_tests/ob_flash_programming.8.res", shallow=False)==True


def test_oneimage():
    runner = CliRunner()
    #py ../AppliCfg.py oneimage -fb "input_tests/app/rot_tz_s_app.bin" -o 0x6000 -sb "input_tests/app/rot_tz_ns_app.bin" -i 0x0 -ob "input_tests/app/rot_tz_app.bin" --vb
    result = runner.invoke(oneimage, ["--first_bin=input_tests/app/rot_tz_s_app.bin", "--optional_size=0x6000",  "--input_size=0x0", "--second_bin=input_tests/app/rot_tz_ns_app.bin",  "--output_bin=input_tests/app/rot_tz_app.bin", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("input_tests/app/rot_tz_app.bin", "input_tests/app/rot_tz_app.res")

def test_sectore_erase():
    runner = CliRunner()
    #py ../AppliCfg.py sectorerase --xml stirot_tests/STiROT_Config.xml --slot primary_code --begin_xml="Firmware installation area address (in external flash)" --size_xml="Firmware area size" --memory ext_nvm --division 0x10000 --vb ob_flash_programming.bat
    shutil.copy("stirot_tests/ob_flash_programming.bat", "temp.bat")
    result = runner.invoke(sectorerase, ["--xml=stirot_tests/STiROT_Config.xml", "--slot=primary_code", "--begin_xml=Firmware installation area address (in external flash)", "--size_xml=Firmware area size","--memory=ext_nvm", "--division=0x10000", "temp.bat", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.bat", "stirot_tests/ob_flash_programming.3.res", shallow=False)==True
 
def test_xml_param():
    runner = CliRunner() 
    shutil.copy("xml_tests/OEMiROT_Code_Image.xml", "temp.xml")
    #doit avoir deux image_macros_preprocessed_bl2, un avec RE_ENCRYPTION=1 et l'autre avec RE_ENCRYPTION=0, doit verifier que on enleve et on enleve bien la balise "Encryption key"
    #py ../AppliCfg.py xmlparam --option=rm --name="Encryption key" --command="-E" --link=GetPublic --type=File --enable=1 --hidden=1 --default="" temp.xml --vb 
    result = runner.invoke(xmlparam, ["--option=rm", "--name=Encryption key", "--command=-E","--link=GetPublic", "--enable=1", "--hidden=1", "--type=File", "temp.xml", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    #shutil.copy("temp.xml", "xml_tests/OEMiROT_Code_Image.4.res")
    assert filecmp.cmp("temp.xml", "xml_tests/OEMiROT_Code_Image.4.res", shallow=False)==True
    #py ../AppliCfg.py xmlparam --option=add --name="Encryption key" --command --link=GetPublic --type=File --command="-E" --enabled=1 --hiden=1 --default="" OEMiROT_Code_Image.xml --vb 
    result = runner.invoke(xmlparam, ["--option=add", "--name=Encryption key", "--command=-E","--link=GetPublic", "--enable=1", "--hidden=1", "--type=File", "temp.xml", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    #shutil.copy("temp.xml", "xml_tests/OEMiROT_Code_Image.5.res")
    assert filecmp.cmp("temp.xml", "xml_tests/OEMiROT_Code_Image.5.res", shallow=False)==True
    shutil.copy("xml_tests/OEMiROT_Code_Image.xml", "temp.xml")
    #RE_OEMUROT_ENABLE n'a pas de sens mais c'est pour trouver une variable a 0, sinon RE_ENCRYPTION
    #py ../AppliCfg.py xmlparam --layout=xml_tests/image_macros_preprocessed_bl2.c --macro=RE_OEMUROT_ENABLE --name="Encryption key" --command="-E" --link=GetPublic --type=File --enable=1 --hidden=1 --default="" temp.xml --vb 
    result = runner.invoke(xmlparam, ["--layout=xml_tests/image_macros_preprocessed_bl2.c","--macro=RE_OEMUROT_ENABLE", "--command=-E","--link=GetPublic", "--enable=1", "--hidden=0", "--type=File", "temp.xml", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    shutil.copy("temp.xml", "xml_tests/OEMiROT_Code_Image.3.res")
    assert filecmp.cmp("temp.xml", "xml_tests/OEMiROT_Code_Image.3.res", shallow=False)==True

def test_xmlen():
    runner = CliRunner() 
    shutil.copy("xml_tests/SM_Code_Image.2.xml", "temp.xml")
    #py ../AppliCfg.py xmlen --name="Dependency with Module 0 image" --enabled=0 temp.xml --vb
    result = runner.invoke(xmlen, ["--name=Dependency with Module 0 image","--enable=0", "temp.xml", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.xml", "xml_tests/SM_Code_Image.3.res", shallow=False)==True
    #py ../AppliCfg.py xmlen --name="Dependency with Module 0 image" --enabled=1 temp.xml --vb
    result = runner.invoke(xmlen, ["--name=Dependency with Module 0 image","--enable=1", "temp.xml", "--vb"])
    print(result.output)
    assert result.exit_code == 0
    assert filecmp.cmp("temp.xml", "xml_tests/SM_Code_Image.2.xml", shallow=False)==True
    
    

if __name__ == '__main__':
    test_Log()
    test_SCRIPT_APPLI_1()
    test_SCRIPT_APPLI_2()
    test_SCRIPT_APPLI_3()
    test_MANAGE_FILE()
    test_BIN_FILE()
    test_setob()
    test_modify()
    test_flash()
    test_xmlval()
    test_linker()
    test_definevalue()
    test_hashcontent()
    test_oneimage()
    test_sectore_erase()
