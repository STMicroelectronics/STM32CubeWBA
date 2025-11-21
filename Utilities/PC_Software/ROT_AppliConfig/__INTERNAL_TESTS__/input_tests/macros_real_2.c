




 














 















 







 

 

     
 





 


 

 

 




 

 

 




 

 
 

 
 
 

 

 



 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 



 
 






 
 
 
 
 


 

 

 

 





 


 
 


 






 



 







 



 


 

 

 

 

 

 

 

 
 


 

 






 





 











 
 




 

 

 
 


 





 



 




 






 

enum image_attributes
{
 
  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE = ((0x0C000)),
  RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE = ((0x1F000)),
 
  RE_IMAGE_FLASH_ADDRESS_SECURE = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))))),
  RE_IMAGE_FLASH_ADDRESS_SECURE_MAGIC_INSTALL_REQ = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))))+((0x06000))-0x10),
  RE_APP_IMAGE_NUMBER = (0x2),
  RE_EXTERNAL_FLASH_ENABLE = (0x0),
  RE_CODE_START_NON_SECURE = (0x0),
  RE_IMAGE_FLASH_ADDRESS_NON_SECURE = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)))),
  RE_IMAGE_FLASH_ADDRESS_NON_SECURE_MAGIC_INSTALL_REQ = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)))+((0xA0000))-0x10),
   
  RE_IMAGE_FLASH_SECURE_UPDATE = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))))),
  RE_IMAGE_FLASH_SECURE_UPDATE_MAGIC_INSTALL_REQ = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))))+((0x06000))-0x10),
  RE_IMAGE_FLASH_NON_SECURE_UPDATE = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))) + ((0x06000)))),
  RE_IMAGE_FLASH_NON_SECURE_UPDATE_MAGIC_INSTALL_REQ = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))) + ((0x06000)))+((0xA0000))-0x10),
  RE_PRIMARY_ONLY = (0x0),
  RE_S_DATA_IMAGE_NUMBER = (0x1),
  RE_IMAGE_FLASH_ADDRESS_DATA_SECURE = ((0x08000000)+((((0x30000))+(0x30000)))),
  RE_IMAGE_FLASH_ADDRESS_DATA_SECURE_MAGIC_INSTALL_REQ = ((0x08000000)+((((0x30000))+(0x30000)))+(((0x2000)))-0x10),
  RE_IMAGE_FLASH_DATA_SECURE_UPDATE = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)))),
  RE_IMAGE_FLASH_DATA_SECURE_UPDATE_MAGIC_INSTALL_REQ = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)))+(((0x2000)))-0x10),
  RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE = (((0x2000))),
  RE_NS_DATA_IMAGE_NUMBER = (0x1),
  RE_IMAGE_FLASH_ADDRESS_DATA_NON_SECURE = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)))),
  RE_IMAGE_FLASH_ADDRESS_DATA_NON_SECURE_MAGIC_INSTALL_REQ = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)))+(((0x2000)))-0x10),
  RE_IMAGE_FLASH_DATA_NON_SECURE_UPDATE = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))))),
  RE_IMAGE_FLASH_DATA_NON_SECURE_UPDATE_MAGIC_INSTALL_REQ = ((0x08000000)+((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))) + ((0x06000)) + ((0xA0000)) + (((0x2000))))+(((0x2000)))-0x10),
  RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE = (((0x2000))),
  RE_BL2_BOOT_ADDRESS = ((((((0x0C000000)) + (((0x30000))))))),
   
   
  RE_BL2_SEC1_END = (((((0x30000))+(0x30000)) + (((0x2000))))+(0x06000)-0x1),
  RE_BL2_SEC2_START = ((0x300000)-0x1),
  RE_BL2_SEC2_END = (0x0),
  RE_BL2_WRP_START = (((0x30000))),
  RE_BL2_HDP_START = 0x0,
  RE_BL2_WRP_END = (((0x30000))+(0x30000)-0x1),
  RE_BL2_HDP_END = (((0x30000))+(0x30000)-0x1),
   

  RE_CRYPTO_SCHEME = 0x0,
  RE_ENCRYPTION = (0x0),

  RE_AREA_0_OFFSET = ((((0x30000))+(0x30000)) + (((0x2000)))),
  RE_AREA_0_SIZE = ((0x06000)),
  RE_AREA_1_OFFSET = ((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000))),
  RE_AREA_1_SIZE = ((0xA0000)),
  RE_AREA_4_OFFSET = ((((0x30000))+(0x30000))),
  RE_AREA_4_SIZE = (((0x2000))),
  RE_AREA_5_OFFSET = ((((0x30000))+(0x30000)) + (((0x2000))) + ((0x06000)) + ((0xA0000))),
  RE_AREA_5_SIZE = (((0x2000))),

  RE_OVER_WRITE = (0x0),
  RE_FLASH_PAGE_NBR = (0x7F)
};
