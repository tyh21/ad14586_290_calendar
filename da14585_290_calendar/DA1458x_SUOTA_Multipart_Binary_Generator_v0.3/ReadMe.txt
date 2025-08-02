#Please install python 3.5

#Application folder architecture contains -

bin\hex2bin.exe
bin\mkimage_580.exe
bin\mkimage_585.exe

input\secondary_bootloader_580.hex
input\secondary_bootloader_585.hex

project_multipart_binary_v2.py

#Please rename your hex and version files according to SUOTA training slides (fw1/fw2).

#Please rename the mkimage_58x.exe to mkimage.exe based on used chip version (580/585).

#Please rename the secondary_bootloader_58x.hex to secondary_bootloader.hex based on used chip version (580/585).
 
#Configure ”user data configuration section” in project_multipart_binary_v2.py

##################### USER DATA CONFIGURATION SECTION #############

#Example secondary bootloader for 580/585 is provided in the input folder.
Please rebuild secondary_bootloader project for proper SPI pin configuration for DA1458x project from 
-\utilities\secondary_bootloader project path in the SDK.
