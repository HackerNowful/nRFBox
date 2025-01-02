<div align="center">

  <img src="https://user-images.githubusercontent.com/62047147/195847997-97553030-3b79-4643-9f2c-1f04bba6b989.png" alt="logo" width="100" height="auto" />
  
  <h1> nRFBOX </h1>
  <p> All-in-One Gadget for BLE and 2.4GHz Networks </p>

## :star2: nRFBox_V2 on HackCat  

If you want to use the **nRFBox_V2** on **HackCat**, follow these steps:  

 Open the `config.h` file.  
 Locate the line:  
   #define nRFBox_V2
 Comment out or remove the line:  
   // #define nRFBox_V2  
   Add the following line to enable HackCat:  
   #define HACKCAT  

Now you’re all set! The nRFBox_V2 is ready to run on the HackCat.


