# DAY 1
# HaikuRansomware

**World's First Haiku Ransomware/Malware**

HaikuRansomware is an experimental ransomware project designed for educational and investigative purposes. Inspired by the art of poetry and the challenge of cryptography, this malware encrypts files with a custom extension and provides a ransom note with a poetic touch. This is a proof of concept aimed to push the boundaries of how creative ransomware can be designed.

### Features
- Encrypts files using AES-256-GCM encryption
- Skips already encrypted files and the ransom note itself
- Places a ransom note in every encrypted folder
- Uses the extension `.utkudorukbayraktarheckledi` for encrypted files

### Proof of Work
![Encryption Proof](assets/proof.png)

### Ransom Note Preview
Every encrypted folder contains a ransom note, **UTKUBABAPROHECKLEDI.txt**, featuring lyrics from the video "[UtkuBabaHeykır Hack'ledi Dünya'yı](https://www.youtube.com/watch?v=y4SYSX-Xgfk)".

The lyrics in the note are:

"0:15\n♪ UtkuBabaHeykır Hack'ledi Dünya'yı ♪\n"
                << "0:19\n♪ Klavyeler gitti, kasalar patladı ♪\n"
                << "0:23\n♪ YouTube kanallarına DDOS atıldı ♪\n"
                << "0:27\n♪ Babapro heykır herkesi Hack'ledi ♪\n"
                << "0:31\n♪ Chat'e yazı yazıp bizi Hack'ledi ♪\n"
                << "0:36\n♪ Kendi bilgisayarına virüs indirdi ♪\n"
                << "0:40\n♪ Baba heykır, pro heykır, babapro heykır, Utkudoruk ♪\n"
                << "1:02\n♪ Utkudoruk, babapro heykır ♪\n"
                << "1:06\n♪ Utkudoruk, baba, baba hiçkır ♪\n"
                << "1:10\n♪ Utkudoruk, babapro heykır ♪\n"
                << "1:14\n♪ Utkudrk, baba baba hiçkır ♪\n"
                << "1:18\n♪ Chat'e yazı yazıp bizi Hack'ledi ♪\n"
                << "1:23\n♪ Kendi bilgisayarına virüs indirdi ♪\n"
                << "1:27\n♪ Baba heykır, pro heykır, babapro heykır, Utkudoruk ♪\n"
                << "1:34\n♪ Baba heykır, pro heykır, babapro heykır, Utkudoruk ♪\n"
                << "1:42\n♪ Baba heykır, pro heykır, babapro heykır, Utkudoruk ♪\n"
                << "\nVideo Reference: https://www.youtube.com/watch?v=y4SYSX-Xgfk\n";

Author: Emirhan Ucan, Utku Doruk Bayraktar (this guy is meme) Project Name: HaikuRansomware Version: 0.2

# DAY 2
# UPDATE
MBR and UEFI payloads added

# References
Learn UEFI: https://github.com/WobbyChip/Delphi/tree/master/MBR%20UEFI%20-%20Note%20Builder
Learn MBR: https://www.youtube.com/watch?v=6hN3q62ttes

# Fun Fact:
This ransomware made in two days. 

## UPDATE It Seems Like Ransomware Working But MBR/UEFI not and you need eject your Haiku ISO
![Encryption Process](assets/v2.png)
![Encryption Process Part2](assets/v2part2.png)
![Encryption Process Part3](assets/v2part3.png)
- https://youtu.be/7yZlYx8h11I

# DAY 3

- https://www.youtube.com/watch?v=cvKeyhhMqFw

- Other related repos: 
- https://github.com/HydraDragonAntivirus/HaikuUEFI
- https://github.com/HydraDragonAntivirus/HydraDragonAVHaiku