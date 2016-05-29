/*****************************************************************************
* Model: blinky.qm
* File:  ./application.h
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/*${.::application.h} ......................................................*/
#ifndef application_h
#define application_h

#include "qpc.h"

enum ApplicationSignals {
    DUMMY_SIG = Q_USER_SIG,
    SERIAL_DEBUG_MSG_SIG,
    MAX_PUB_SIG,          /* the last published signal */

    TIMEOUT_SIG,
    MAX_SIG               /* the last signal */
};

/*${Events::SerialDebugMsgEvt} .............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint8_t data[100];
} SerialDebugMsgEvt;


#endif /* application_h */
