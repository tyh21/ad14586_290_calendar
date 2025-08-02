#! /usr/bin/python
'''
	file name 	:: project_multipart_binary_v2.py
	author 		:: m alam
	version 	:: sw_v_0000.00003
	date		:: 2017.Jan.11
	python ver  :: 3.5.1
	description :: This script can be used to genereate Dialog DA1458x specific multi_part bin file
	reference   :: UM-B-012_DA14580-581-583 creation of secondary boot loader
				   AN-B-10 DA1458x using SUOTA
	disclaimer  :: This script is free to use, any modification can be done on own risk.
				   
	Instruction :: Please modify USER DATA CONFIGURATION SECTION according to your needs and specification.
'''

import sys
import os.path
import glob


##################### USER DATA CONFIGURATION SECTION #############
# TODO user :: 
# Set firmware 1 hex file name and firmware 1 software version header file name without extension
# default FW_1			= "fw_1"
# default FW_1_SW_VER 	= "fw_1_version"
FW_1			= "fw_1"
FW_1_SW_VER 	= "fw_1_version"

# TODO user:: 
# Set firmware 2 hex file name and firmware 2 software version header file name without extension
# default FW_1			= "fw_2"
# default FW_1_SW_VER 	= "fw_2_version"
FW_2			= "fw_2"
FW_2_SW_VER 	= "fw_2_version"

# TODO user:: 
# Set secondary bootloader hex file name without extension
# default "secondary_bootloader"
BOOT_2ND_LOADER = "secondary_bootloader"


# TODO user:: 
# Set True or False; 
# if True then multi partition binary file is to be burnt in OTP; therefore 2ndary bootloader will NOT be stored in external memory
# if False then multi partition binary file is not burnt in OTP; therefore 2ndary bootloader will be stored in external memory
# default False
BOOT_2ND_LOADER_IN_OTP = False

# TODO user::
# Set 'spi' 	(lowercase only) to create multi partition binary file to load in spi external memory interface
# Set 'eeprom' 	(lowercase only) to create multi partition binary file to load in eeprom external memory 
# default 'spi'
EXTERNAL_MEMORY = "spi"

# TODO user::
# Set memory location offesets for firmware 1 image, firmware 2 image and product header
# default MEM_LOC_FOR_FW_1_IMG 		= "0x8000"
# default MEM_LOC_FOR_FW_2_IMG 		= "0x13000"
# default MEM_LOC_FOR_PRODUCT_HEADER 	= "0x1F000" 
MEM_LOC_FOR_FW_1_IMG 		= "0x2000"
MEM_LOC_FOR_FW_2_IMG 		= "0x14000"
MEM_LOC_FOR_PRODUCT_HEADER 	= "0x38000"


# TODO user:: 
# Set True or False; if True then output image file will be created with default encryption key and init vector value;
# SEE IMG_ENC_KEY_DEF and IMG_ENC_INIT_VEC_DEF
# default both False
IMG_1_ENC		= False
IMG_2_ENC		= False

# TODO user:: 
# Set True or False; if True then output image file will be created with user defined encryption key and init vector value;
# TODO user:: if True then assign corresponding user defined key and init vector value
# exmple:: 
# IMG_1_ENC_MANUAL 		= True
# IMG_ENC_KEY_1 		= "AAAABBBBCCCCDDDDEEEEFFFF00001111"
# IMG_ENC_INIT_VEC_1	= "AAAABBBBCCCCDDDDEEEEFFFF11112222"
# default both False
IMG_1_ENC_MANUAL 	= False
IMG_2_ENC_MANUAL 	= False

# TODO user:: 
# if IMG_1_ENC is set True; AND IMG_1_ENC_MANUAL is set True then
# set 32 byte user defined number check IMG_ENC_KEY_DEF, IMG_ENC_INIT_VEC_DEF for number format
# default both ""
IMG_ENC_KEY_1 		= ""
IMG_ENC_INIT_VEC_1	= ""

# TODO user::
# if IMG_2_ENC is set True; AND IMG_2_ENC_MANUAL is set True then
# set 32 byte user defined number check IMG_ENC_KEY_DEF, IMG_ENC_INIT_VEC_DEF for number format
# default both ""
IMG_ENC_KEY_2 		= ""
IMG_ENC_INIT_VEC_2	= ""

##################### USER DATA CONFIGURATION SECTION ENDS #############




##################### DO NOT EDIT BELOW ################################
IMG_ENC_KEY_DEF 		= "06A9214036B8A15B512E03D534120006"
IMG_ENC_INIT_VEC_DEF	= "3DAFBA429D9EB430B422DA802C9FAC41"

OUTPUT_IMG_1			= "fw_image_1"
OUTPUT_IMG_2			= "fw_image_2"


HEX = ".hex"
H	= ".h"
BIN = ".bin"
IMG = ".img"

SINGLE = "single"
MULTI = "multi"


SPACE = " "

KEY_LENGTH = 32


#check user data configuration
def check_configuration():	
	
	if IMG_1_ENC == True and IMG_1_ENC_MANUAL == True:
		if IMG_ENC_KEY_1 == IMG_ENC_KEY_DEF and IMG_ENC_INIT_VEC_1 == IMG_ENC_INIT_VEC_DEF:
			print("############# :: ERROR :: ###################")
			print("IMG_1_ENC_MANUAL is set to True.")				
			print("Assign IMG_ENC_KEY_1 or IMG_ENC_INIT_VEC_1 or both with user defined "+ str(KEY_LENGTH) +" bit hexadecimal number.")				
			print("#############################################")
			return False	
			
		if len(IMG_ENC_KEY_1) != KEY_LENGTH or len(IMG_ENC_INIT_VEC_1) != KEY_LENGTH:
			print("############# :: ERROR :: ###################")
			print("IMG_1_ENC_MANUAL is set to True.")				
			print("Assign IMG_ENC_KEY_1 or IMG_ENC_INIT_VEC_1 or both with user defined hexadecimal number of length "+ str(KEY_LENGTH) +".")				
			print("#############################################")
			return False		
		
	if IMG_2_ENC == True and IMG_2_ENC_MANUAL == True:
		if IMG_ENC_KEY_2 == IMG_ENC_KEY_DEF and IMG_ENC_INIT_VEC_2 == IMG_ENC_INIT_VEC_DEF:
			print("############# :: ERROR :: ###################")
			print("IMG_2_ENC_MANUAL is set to True.")				
			print("Assign IMG_ENC_KEY_2 or IMG_ENC_INIT_VEC_2 or both with user defined "+ str(KEY_LENGTH) +" bit hexadecimal number.")				
			print("#############################################")
			return False
			
		if len(IMG_ENC_KEY_2) != KEY_LENGTH or len(IMG_ENC_INIT_VEC_2) != KEY_LENGTH:
			print("############# :: ERROR :: ###################")
			print("IMG_2_ENC_MANUAL is set to True.")				
			print("Assign IMG_ENC_KEY_2 or IMG_ENC_INIT_VEC_2 or both with user defined hexadecimal number of length "+ str(KEY_LENGTH) +".")				
			print("#############################################")
			return False
	
	
	res	= os.path.isfile("bin\hex2bin.exe")
	if (res == False):
		print("Error:: hex2bin.exe not found. Copy file to 'bin' directory")
		print(os.path.abspath("bin") + "\hex2bin.exe")
		exit()
		
	res	= os.path.isfile("bin\mkimage.exe")
	if (res == False):
		print("Error:: mkimage.exe not found. Copy file to 'bin' directory")
		print(os.path.abspath("bin") + "\mkimage.exe")
		exit()
		
	res	= os.path.isfile("input\\"+FW_1+HEX)
	if (res == False):
		print("Error:: "+ FW_1 + HEX +" not found. Copy file to 'input' directory")
		print(os.path.abspath("input\\") + FW_1 + HEX)
		exit()
		
	res	= os.path.isfile("input\\"+FW_1_SW_VER + H)
	if (res == False):
		print("Error:: "+ FW_1_SW_VER + H + " not found. Copy file to 'input' directory")
		print(os.path.abspath("input\\") + FW_1_SW_VER + H)
		exit()
		
	res	= os.path.isfile("input\\"+FW_2 + HEX)
	if (res == False):
		print("Error:: "+ FW_2 + HEX +" not found. Copy file to 'input' directory")
		print(os.path.abspath("input\\") + FW_2 + HEX)
		exit()
		
	res	= os.path.isfile("input\\"+FW_2_SW_VER + H)
	if (res == False):
		print("Error:: "+ FW_2_SW_VER + H +" not found. Copy file to 'input' directory")
		print(os.path.abspath("input\\") + FW_2_SW_VER + H)
		exit()
		
	res	= os.path.isfile("input\\"+BOOT_2ND_LOADER + HEX)
	if (res == False):
		print("Error:: "+ BOOT_2ND_LOADER + HEX +" not found. Copy file to 'input' directory")
		print(os.path.abspath("input\\") + BOOT_2ND_LOADER + HEX)
		exit()
	
	files = glob.glob('input\*.bin')
	for f in files:
		os.remove(f)
	
	
	res	= os.path.exists("output")
	if(res == True):
		files = glob.glob('output\*')
		for f in files:
			os.remove(f)
		os.rmdir("output")
		
	os.mkdir("output");
	return True
	
# convert hex file to bin file
def convert_hex_2_bin():
	os.system("bin\hex2bin.exe" + SPACE + "-e bin" + SPACE + "input\\" + FW_1 + HEX)
	os.system("bin\hex2bin.exe" + SPACE + "-e bin" + SPACE + "input\\" + FW_2 + HEX)
	os.system("bin\hex2bin.exe" + SPACE + "-e bin" + SPACE + "input\\" + BOOT_2ND_LOADER + HEX)
	
	os.rename("input\\" + FW_1 + BIN, "output\\" + FW_1 + BIN)
	os.rename("input\\" + FW_2 + BIN, "output\\" + FW_2 + BIN)
	os.rename("input\\" + BOOT_2ND_LOADER + BIN, "output\\" + BOOT_2ND_LOADER + BIN)
	
# create dialog DA1458x specific images; this is a generic api
def bin_to_img(img_enc, img_enc_manual, fware, fware_sw_version, output_img_name, img_enc_key, img_enc_init_vec):
	if img_enc == False:		
		os.system("bin\mkimage.exe" + SPACE + SINGLE + SPACE + "output\\" + fware + BIN + SPACE + "input\\" + fware_sw_version + H + SPACE + "output\\" + output_img_name + IMG)
		print ("INFO:: "+ output_img_name + IMG + SPACE +" is successfully created with no data encryption.")
	else:
		if img_enc_manual == False:
			os.system("bin\mkimage.exe" + SPACE + SINGLE + SPACE + "output\\" + fware + BIN + SPACE + "input\\" + fware_sw_version + H + SPACE + "output\\" + output_img_name + IMG + SPACE + "enc")
			print ("INFO:: "+ output_img_name + IMG + SPACE +" is successfully encrypted with default key value " + IMG_ENC_KEY_DEF)
			print ("INFO:: "+ output_img_name + IMG + SPACE +" is successfully encrypted with default init vector value " + IMG_ENC_INIT_VEC_DEF)			
		else:			
			os.system("bin\mkimage.exe" + SPACE + SINGLE + SPACE + "output\\" + fware + BIN + SPACE + "input\\" + fware_sw_version + H + SPACE + "output\\" + output_img_name + IMG + SPACE + "enc" + SPACE + img_enc_key + SPACE + img_enc_init_vec)
			print ("INFO:: "+ output_img_name + IMG + SPACE +" is successfully encrypted with user defined key value " + img_enc_key)
			print ("INFO:: "+ output_img_name + IMG + SPACE +" is successfully encrypted with user defined init vector value " + img_enc_init_vec)	

# create dialog DA1458x specific images
def convert_bin_2_img():
	print("\n")
	bin_to_img(IMG_1_ENC, IMG_1_ENC_MANUAL, FW_1, FW_1_SW_VER, OUTPUT_IMG_1, IMG_ENC_KEY_1, IMG_ENC_INIT_VEC_1)
	bin_to_img(IMG_2_ENC, IMG_2_ENC_MANUAL, FW_2, FW_2_SW_VER, OUTPUT_IMG_2, IMG_ENC_KEY_2, IMG_ENC_INIT_VEC_2)
	print("\n")

# create dialog DA1458x specific multi part binary	
def create_multi_part_bin():
	if EXTERNAL_MEMORY == "spi":
		OUTPUT_MULTI_PART = "fw_multi_part_spi"
	elif EXTERNAL_MEMORY == "eeprom":
		OUTPUT_MULTI_PART = "fw_multi_part_eeprom"
	else:
		print("############# :: ERROR :: ###################")
		print("Assign EXTERNAL_MEMORY has to be either with 'spi' or 'eeprom'. Check user configuration in project_img.py file")				
		print("#############################################")
		return False
	if BOOT_2ND_LOADER_IN_OTP == False:	
		res = os.system("bin\mkimage.exe" + SPACE + MULTI + SPACE + EXTERNAL_MEMORY + SPACE + "output\\" + BOOT_2ND_LOADER + BIN + SPACE + "output\\" + OUTPUT_IMG_1 + IMG + SPACE + MEM_LOC_FOR_FW_1_IMG + SPACE + "output\\" + OUTPUT_IMG_2 + IMG + SPACE + MEM_LOC_FOR_FW_2_IMG + SPACE + MEM_LOC_FOR_PRODUCT_HEADER + SPACE + "output\\" + OUTPUT_MULTI_PART + BIN)
	else:
		res = os.system("bin\mkimage.exe" + SPACE + MULTI + SPACE + EXTERNAL_MEMORY + SPACE + "output\\" + OUTPUT_IMG_1 + IMG + SPACE + MEM_LOC_FOR_FW_1_IMG + SPACE + "output\\" + OUTPUT_IMG_2 + IMG + SPACE + MEM_LOC_FOR_FW_2_IMG + SPACE + MEM_LOC_FOR_PRODUCT_HEADER + SPACE + "output\\" + OUTPUT_MULTI_PART + BIN)
		
	if res == 0:
		print (OUTPUT_MULTI_PART + BIN + SPACE + " is created successfully.")
		return True
	else:
		print("############# :: ERROR :: ##############################################")
		print (OUTPUT_MULTI_PART + BIN + SPACE + " is not created. Check configuration.")
		print("########################################################################")
		return False
		
	return True

# check script configuration is set properly or not
# If configuration is set correctly then covert hex to bin, bin to img and finally create multipart binary
def run_application():
	
	os.system('cls')
	print("############# FREE SCRIPT FOR CREATING DA1458x MULTI PART IMAGE   ###################")	
	print("################### version 	:: sw_v_0000.00003 ##################################")
	print("##################### Developed in 2017 Janua #######################################")
	
	res = check_configuration()
	if res == True:
		convert_hex_2_bin()
		convert_bin_2_img()
		res = create_multi_part_bin()
		
	else:
		print("############# :: ERROR :: ###################")
		print("Configure user data correctly in project_img.py")
		print("#############################################")
		
	print("#####################################################################################")

# start application
if __name__ == "__main__":	
	run_application()

