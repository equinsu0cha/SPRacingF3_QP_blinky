/*****************************************************************************
* Product: "Blinky" example
* Last Updated for Version: 5.4.0
* Date of the Last Update:  2015-03-07
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. state-machine.com.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Web  : http://www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#include "qpc.h"
#include "bsp.h"
#include "application.h"

#include "blinky.h"
#include "serial_debug.h"

/*..........................................................................*/
int main() {
    static QEvt const *l_blinkyQSto[10];   /* Event queue storage for Blinky */
    static QEvt const *l_serial_debug_queue_sto[1]; /* Event queue size one for UART1 */
    static QSubscrList subscrSto[MAX_PUB_SIG];
    static QF_MPOOL_EL(SerialDebugMsgEvt) smlPoolSto[2]; /* small pool */

    QF_init();  /* initialize the framework and the underlying RT kernel */
    BSP_init(); /* initialize the Board Support Package */

    /* initialize publish-subscribe... */
    QF_psInit(subscrSto, Q_DIM(subscrSto));

    /* initialize event pools... */
    QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    /* instantiate and start the active objects... */
    SerialDebug_ctor();
    QACTIVE_START(AO_SerialDebug,
                  2U,
                  l_serial_debug_queue_sto,
                  Q_DIM(l_serial_debug_queue_sto),
                  (void *)0,      /* stack storage (not used in QK) */
                  0U,             /* stack size [bytes] (not used in QK) */
                  (QEvt *)0);     /* initial event (or 0) */

    Blinky_ctor();
    QACTIVE_START(AO_Blinky,      /* AO pointer to start */
                  1U,             /* unique QP priority of the AO */
                  l_blinkyQSto,   /* storage for the AO's queue */
                  Q_DIM(l_blinkyQSto), /* lenght of the queue [entries] */
                  (void *)0,      /* stack storage (not used in QK) */
                  0U,             /* stack size [bytes] (not used in QK) */
                  (QEvt *)0);     /* initial event (or 0) */

    return QF_run(); /* run the QF application */
}
