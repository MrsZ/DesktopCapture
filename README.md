# DesktopCapture
A application which can capture automatically or by keyboard shortcuts without loss focus  from present application
***
####Develop tools: 
Microsoft Visual Studio 2015

####Project type: 
MFC project based on dialog  

---
####Help:
* Shortcut
  1. <strong>ctrl+alt+0</strong>　　capture current screen and save it automatically to appointed folder  
　　　　　　　(截取当前屏幕并保存到指定目录)
  2. <strong>ctrl+alt+9</strong>　　close the application, like the function of press down ESC key on the main dialog  
　　　　　　　(关闭该程序，功能类似于在左下角的主窗口上按下ESC)
  3. <strong>ctrl+alt+1</strong>　　open the appointed folder, like the function of double-click on the main dialog  
　　　　　　　(打开存放截图的文件夹，功能类似于在左下角的主窗口上双击鼠标左键)
  4. <strong>ctrl+alt+2</strong>　　stop/start capture automatically, reserve manual capture  
　　　　　　　(停止/开启设定的自动截图，但保留手动快捷键截图功能，再按一次可切换模式)
* configuration file
  1. <strong>Sleep</strong>　　　　Sleep time, that is, automatic time interval    
　　　　　　　(休眠时间,即自动截图时间间隔)    
  2. <strong>fatherFolder</strong>　the parent directory of screenshot file location , the program will generate    
　　　　　　　a sub folder in the directory with current date     
　　　　　　　(截图文件位置的父目录，程序会在该目录下生成一个当前日期的子文件夹)
