




 














 















 







 

 



 

        

 
                                             
      
 

 

 

 

 

 




 

 

 
 

 
 

 

 

 
 

 



 

 
 

 

 

 
 

 
 

 
 



 
 
 


 

 




 


 


 






 



 







 




 

 

 

 

 

 

 

 

 
 


 
      
 




 






 





 







 





 











 
 




 

 

 
 


 


                                      



 



 




 






 

enum image_attributes
{
   
  RE_CODE_IMAGE_FLASH_SIZE = ((0xE000)),
  RE_DATA_IMAGE_FLASH_SIZE = ((0x2000)),

   
  RE_AREA_0_OFFSET = (((0x20000))),
  
  RE_AREA_5_OFFSET = (((0x20000) + ((0xE000)))),

  RE_AREA_2_OFFSET = (((0x20000) + ((0xE000)) + ((0x2000)))),

  RE_AREA_7_OFFSET = (((0x20000) + ((0xE000)) + ((0xE000)) + ((0x2000)))),

  RE_IMAGE_PRIMARY_AREA_OFFSET = (((((0x20000))) + (0x400))),

  RE_DATA_IMAGE_NUMBER = (0x1),
  RE_DATA_IMAGE_ADDRESS = ((0x08000000) + ((0x20000) + ((0xE000)))),
  RE_DATA_OFFSET = (((0x20))),
  RE_DATA_SIZE = ((0x20)),

  RE_CODE_IMAGE_ADDRESS = ((0x08000000) + ((0x20000))),

  RE_BL2_PERSO_ADDRESS = ((0x08000000) + ((0x0) + (0x10000))),
  RE_BL2_BOOT_ADDRESS = ((((((0x08000000)) + ((0x0)))))),
  
   
  RE_BL2_WRP_END = (((0x0) + (0x10000)) + (0x2000) - 0x1),
  
  RE_LOADER_WRP_START = ((((((0x0) + (0x10000)) + (0x2000)) + (0x1000)) + (0x1000) + (0x8000))),
  RE_LOADER_WRP_END = ((((((0x0) + (0x10000)) + (0x2000)) + (0x1000)) + (0x1000) + (0x8000)) + (0x4000) -1),
  
  RE_BL2_HDP_END = ((((((0x0) + (0x10000)) + (0x2000)) + (0x1000)) + (0x1000)) + (0x8000) - 0x1),

  RE_FLASH_AREA_LOADER_OFFSET = ((((((0x0) + (0x10000)) + (0x2000)) + (0x1000)) + (0x1000) + (0x8000))),
  RE_LOADER_CODE_START = ((((0x08000000) + ((((((0x0) + (0x10000)) + (0x2000)) + (0x1000)) + (0x1000) + (0x8000)))))),
  RE_LOADER_CODE_SIZE = ((0x4000)),

  RE_PRIMARY_ONLY = (0x0),
  RE_IMAGE_PARTITION_OFFSET = (((0x20000) + ((0xE000)) + ((0x2000)))),
  RE_IMAGE_PARTITION_ADDRESS = ((0x08000000) + ((0x20000) + ((0xE000)) + ((0x2000)))),
  RE_DATA_IMAGE_PARTITION_OFFSET = (((0x20000) + ((0xE000)) + ((0xE000)) + ((0x2000)))),
  RE_DATA_IMAGE_PARTITION_ADDRESS = ((0x08000000) + ((0x20000) + ((0xE000)) + ((0xE000)) + ((0x2000)))),

  RE_OVER_WRITE = (0x0),

  RE_FLASH_PAGE_NBR = (0x80)
};
