
<div align="center">
  <a href="https://lean-sat.org/opensource/">
  <img alt="Join the monthly community webinars" src="https://img.shields.io/badge/join_our-monthly_webinars-orange" />
  </a>
</div>

<img  width="25%" src="https://github.com/BIRDSOpenSource/BIRDS4-ProceduresAndReports/assets/100206676/055720a1-5845-43c9-b26e-dfcc6dc78b23"> <img width="40%" src="https://github.com/BIRDSOpenSource/BIRDS4-ProceduresAndReports/assets/100206676/4adeca5e-c683-47f9-b4a4-c95ebf40f177">

# BIRDS4-OBC
 Documentation for the On Board Computer used in the BIRDS4 satellite


## About the BIRDS4 Program:

The Mission Statement of the Project is:

> “By successfully building and operating first satellite of nation, advance toward indigenous space program in each country”

The project is led by Kyushu Institute of Technology in Japan and involves students from Philippines, Paraguay, Nepal, Turkey and Japan.

Satellite Names: Tsuru (Japan), Guarani-1 (Paraguay) and Maya-2 (Phillipines)

Stakeholders:  Paraguayan Space Agency, Polytechnic Faculty of National University of Asuncion, and Kyushu Institute of Technology (Kyutech) for Japan.

Project duration: 2018-2022

CubeSat: 1U (10x10x10) cm3

Weight: 1.13kg

Launch Date: 21st February 2021 

BIRDS-4 has 1 main missions in total; APRS Store and forward mission

## Organisation structure
```bash
├── .gitattributes
├── BIRDS3-4 OBC Schematic.pdf
├── BIRDS4_COM_PIC_FM_FIRMWARE
│   ├── CMD_FUN.c
│   ├── COMPIC_CMMANDS.c
│   ├── COMPIC_FUN.c
│   ├── COM_FLASH.c
│   ├── COM_PIC_FM_JP.c
│   ├── COM_PIC_FM_JP.ccspjt
│   ├── COM_PIC_FM_JP.h
│   ├── COM_PIC_FM_JP.hex
│   ├── CW_JP.c
│   ├── PACKET_CODE_JP.c
│   └── compicreg.h
├── BIRDS4_MAIN_PIC_FM_FIRMWARE
│   ├── AUTO_MSN.c
│   ├── FLASH_OPERATION.c
│   ├── MAIN_PIC_FM Notes.txt
│   ├── MAIN_PIC_FM.STA
│   ├── MAIN_PIC_FM.bak
│   ├── MAIN_PIC_FM.c
│   ├── MAIN_PIC_FM.ccspjt
│   ├── MAIN_PIC_FM.ccspjt.err
│   ├── MAIN_PIC_FM.ccspjt.esym
│   ├── MAIN_PIC_FM.cof
│   ├── MAIN_PIC_FM.err
│   ├── MAIN_PIC_FM.esym
│   ├── MAIN_PIC_FM.h
│   ├── MAIN_PIC_FM.hex
│   ├── MAIN_PIC_FM.lst
│   ├── MAIN_PIC_FM.sym
│   ├── MAIN_PIC_FM.tre
│   ├── MAIN_PIC_FM.xsym
│   ├── MEMORY_OPERATION.c
│   ├── PACKET_CODE_JPN.c
│   ├── PIC18F67J94_registers.h
│   ├── SATELLITE_LOG.c
│   ├── analyze_command.c
│   ├── flash_ memory_ MT25QL01GBBB_OF.c
│   ├── mainpic_function.c
│   ├── mainpic_function.h
│   └── reserve_function.c
├── BIRDS4_RESET_PIC_FM_FIRMWARE
│   ├── PIC16F1789_registers.h
│   ├── RESET_PIC_FM.c
│   ├── RESET_PIC_FM.ccspjt
│   ├── RESET_PIC_FM.h
│   ├── RESET_PIC_FM.hex
│   ├── analyze_command.c
│   ├── define_fun.c
│   ├── power_enable.c
│   └── read_current.c
├── LICENSE
├── OBC BOM.xlsx
└── README.md

```

## Features


## I like this project, can I help you?
You are welcome 🙂

* Give us a star ⭐
* Watch repository to be notified about updates 👀
* If you will find some errors, report them in Issues 🐞
* Try Birds designs on some real computer and let us know how it worked in Discussions. 💬
* We would greatly appreciate if you would also tell others about this ecosystem, it helps us to improve BIRDS Open Source. Thank you!
