The current firmware is built with the CCS PCWHD C-Aware IDE
http://www.ccsinfo.com/product_info.php?products_id=PCWHD_full
Although they have not been tested, the PCDIDE C-Aware IDE
and PCD command line compilers should work for building the
firmware.

Further development with the CCS compilers will be minimal and
limited to bug fixes and small feature additions.

Currently, we are developing a bootloader based on Microchip's
bootloader.  This includes a change to MPLAB X IDE and PICkit 4
In-Circuit Debugger.  The intention is to switch to free tools
as much as possible, as well as expediting the upgrade process
through the switch to a USB bootloader.
