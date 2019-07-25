#This powershell script prepares your Windows 10 64-bit computer to be able to compile your
#Prusa Firmware by installing a Linux Subsystem
#Please run it as administrator

Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux
Invoke-WebRequest -Uri https://aka.ms/wsl-ubuntu -OutFile Ubuntu.appx -UseBasicParsing