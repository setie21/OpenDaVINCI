/**
 * odplayer - Tool for playing back recorded data
 * Copyright (C) 2017 Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PLAYER2TESTSUITE_H_
#define PLAYER2TESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <fstream>
#include <iostream>
#include <map>
#include <limits>
#include <vector>

#include <opendavinci/odcore/base/Thread.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/io/URL.h>
#include <opendavinci/odtools/player/Player2.h>

using namespace std;
using namespace odcore::base;
using namespace odcore::data;
using namespace odcore::io;
using namespace odtools::player;

class PlayerModule2Test : public CxxTest::TestSuite {
    public:
        void notestEmptyFileAndException() {
            cout << __FILE__ << " " << __LINE__ << endl;
            // Prepare record file.
            fstream fout("PlayerModule2TestEmpty.rec", ios::out | ios::binary | ios::trunc);
            fout.flush();
            fout.close();

            const URL u("file://PlayerModule2TestEmpty.rec");

            // Create Player2 instance.
            const bool NO_AUTO_REWIND = false;
            Player2 p2(u, NO_AUTO_REWIND);

            while (p2.hasMoreData()) {
            }

            bool exceptionCaught = false;
            try {
                const Container& c55 = p2.getNextContainerToBeSent();
                (void)c55;
            }
            catch(...) {
                exceptionCaught = true;
            }

            TS_ASSERT(exceptionCaught);

            UNLINK("PlayerModule2TestEmpty.rec");
        }

        void notestCorrectMonotonicTemporalOrderAndExceptionFromReverseOrder() {
            cout << __FILE__ << " " << __LINE__ << endl;
            // Prepare record file.
            fstream fout("PlayerModule2Test.rec", ios::out | ios::binary | ios::trunc);

            // Write data with non-monotonic order.
            TimeStamp ts1(4, 5);
            TimeStamp ts2(3, 4);
            TimeStamp ts3(2, 3);
            TimeStamp ts4(1, 2);
            TimeStamp ts5(0, 1);

            Container c1(ts1);
            c1.setSampleTimeStamp(ts1);
            fout << c1;

            Container c2(ts2);
            c2.setSampleTimeStamp(ts2);
            fout << c2;

            Container c3(ts3);
            c3.setSampleTimeStamp(ts3);
            fout << c3;

            Container c4(ts4);
            c4.setSampleTimeStamp(ts4);
            fout << c4;

            Container c5(ts5);
            c5.setSampleTimeStamp(ts5);
            fout << c5;

            fout.flush();
            fout.close();

            const URL u("file://PlayerModule2Test.rec");

            // Create Player2 instance.
            const bool NO_AUTO_REWIND = false;
            Player2 p2(u, NO_AUTO_REWIND);

            TimeStamp before;
            int64_t counter = 0;
            while (p2.hasMoreData()) {
                const Container& c = p2.getNextContainerToBeSent();
                if (counter == 0) {
                    TS_ASSERT(p2.getDelay() == 0);
                }
                else {
                    TS_ASSERT(p2.getDelay() == 1000001);
                }

                TS_ASSERT((counter * 1000 * 1000 + (counter + 1)) == c.getSampleTimeStamp().toMicroseconds());
                counter++;
            }
            TimeStamp after;
            cout << "Duration = " << (after - before).toMicroseconds() << endl;

            bool exceptionCaught = false;
            try {
                const Container& c55 = p2.getNextContainerToBeSent();
                (void)c55;
            }
            catch(...) {
                exceptionCaught = true;
            }

            TS_ASSERT(exceptionCaught);

            UNLINK("PlayerModuleTest2.rec");
        }

        void notestCorrectMonotonicTemporalOrderAndExceptionFromReverseOrderRealtimeReplay() {
            cout << __FILE__ << " " << __LINE__ << endl;
            // Prepare record file.
            fstream fout("PlayerModule2Test.rec", ios::out | ios::binary | ios::trunc);

            // Write data with non-monotonic order.
            TimeStamp ts0(5, 6);
            TimeStamp ts1(4, 5);
            TimeStamp ts2(3, 4);
            TimeStamp ts3(2, 3);
            TimeStamp ts4(1, 2);

            Container c0(ts0);
            c0.setSampleTimeStamp(ts0);
            fout << c0;

            Container c1(ts1);
            c1.setSampleTimeStamp(ts1);
            fout << c1;

            Container c2(ts2);
            c2.setSampleTimeStamp(ts2);
            fout << c2;

            Container c3(ts3);
            c3.setSampleTimeStamp(ts3);
            fout << c3;

            Container c4(ts4);
            c4.setSampleTimeStamp(ts4);
            fout << c4;

            fout.flush();
            fout.close();

            const URL u("file://PlayerModule2Test.rec");

            // Create Player2 instance.
            const bool NO_AUTO_REWIND = false;
            Player2 p2(u, NO_AUTO_REWIND);

            TimeStamp before;
            int64_t counter = 1;
            while (p2.hasMoreData()) {
                const Container& c = p2.getNextContainerToBeSent();
                if (counter > 1) {
                    TS_ASSERT(p2.getDelay() == 1000001);
                }

                Thread::usleepFor(p2.getDelay());

                TS_ASSERT((counter * 1000 * 1000 + (counter + 1)) == c.getSampleTimeStamp().toMicroseconds());
                counter++;
            }
            TimeStamp after;
            cout << "Duration = " << (after - before).toMicroseconds() << endl;

            bool exceptionCaught = false;
            try {
                const Container& c55 = p2.getNextContainerToBeSent();
                (void)c55;
            }
            catch(...) {
                exceptionCaught = true;
            }

            TS_ASSERT(exceptionCaught);

            UNLINK("PlayerModuleTest2.rec");
        }

        void notestCorrectMonotonicTemporalOrderAndExceptionFromReverseOrderWithDuplicateEntries() {
            cout << __FILE__ << " " << __LINE__ << endl;
            // Prepare record file.
            fstream fout("PlayerModule2Test.rec", ios::out | ios::binary | ios::trunc);

            // Write data with non-monotonic order.
            TimeStamp ts1(4, 5);
            TimeStamp ts2(3, 4);
            TimeStamp ts3(2, 3);
            TimeStamp ts4(1, 2);
            TimeStamp ts5(0, 1);
            TimeStamp ts6(2, 3);

            Container c1(ts1);
            c1.setSampleTimeStamp(ts1);
            fout << c1;

            Container c2(ts2);
            c2.setSampleTimeStamp(ts2);
            fout << c2;

            Container c3(ts3);
            c3.setSampleTimeStamp(ts3);
            fout << c3;

            Container c4(ts4);
            c4.setSampleTimeStamp(ts4);
            fout << c4;

            Container c5(ts5);
            c5.setSampleTimeStamp(ts5);
            fout << c5;

            Container c6(ts6);
            c6.setSampleTimeStamp(ts6);
            fout << c6;

            fout.flush();
            fout.close();

            const URL u("file://PlayerModule2Test.rec");

            // Create Player2 instance.
            const bool NO_AUTO_REWIND = false;
            Player2 p2(u, NO_AUTO_REWIND);

            TimeStamp before;
            int64_t counter = 0;
            while (p2.hasMoreData()) {
                const Container& c = p2.getNextContainerToBeSent();
                if (counter == 0) {
                    TS_ASSERT(p2.getDelay() == 0);
                }
                else if (counter == 3) {
                    TS_ASSERT(p2.getDelay() == 0);
                }
                else {
                    TS_ASSERT(p2.getDelay() == 1000001);
                }

                if (counter < 3) {
                    TS_ASSERT((counter * 1000 * 1000 + (counter + 1)) == c.getSampleTimeStamp().toMicroseconds());
                }
                if (counter == 3) {
                    TS_ASSERT(c.getSampleTimeStamp().toMicroseconds() == 2000003);
                }
                if (counter > 3) {
                    TS_ASSERT(((counter-1) * 1000 * 1000 + ((counter-1) + 1)) == c.getSampleTimeStamp().toMicroseconds());
                }
                counter++;
            }
            TimeStamp after;
            cout << "Duration = " << (after - before).toMicroseconds() << endl;

            bool exceptionCaught = false;
            try {
                const Container& c55 = p2.getNextContainerToBeSent();
                (void)c55;
            }
            catch(...) {
                exceptionCaught = true;
            }

            TS_ASSERT(exceptionCaught);

            UNLINK("PlayerModuleTest2.rec");
        }

        void notestCorrectMonotonicTemporalOrderAndException() {
            cout << __FILE__ << " " << __LINE__ << endl;
            // Prepare record file.
            fstream fout("PlayerModule2Test.rec", ios::out | ios::binary | ios::trunc);

            // Write data with non-monotonic order.
            TimeStamp ts1(4, 5);
            TimeStamp ts2(2, 3);
            TimeStamp ts3(0, 1);
            TimeStamp ts4(1, 2);
            TimeStamp ts5(3, 4);

            Container c1(ts1);
            c1.setSampleTimeStamp(ts1);
            fout << c1;

            Container c2(ts2);
            c2.setSampleTimeStamp(ts2);
            fout << c2;

            Container c3(ts3);
            c3.setSampleTimeStamp(ts3);
            fout << c3;

            Container c4(ts4);
            c4.setSampleTimeStamp(ts4);
            fout << c4;

            Container c5(ts5);
            c5.setSampleTimeStamp(ts5);
            fout << c5;

            fout.flush();
            fout.close();

            const URL u("file://PlayerModule2Test.rec");

            // Create Player2 instance.
            const bool NO_AUTO_REWIND = false;
            Player2 p2(u, NO_AUTO_REWIND);

            TimeStamp before;
            int64_t counter = 0;
            while (p2.hasMoreData()) {
                const Container& c = p2.getNextContainerToBeSent();
                if (counter == 0) {
                    TS_ASSERT(p2.getDelay() == 0);
                }
                else {
                    TS_ASSERT(p2.getDelay() == 1000001);
                }

                TS_ASSERT((counter * 1000 * 1000 + (counter + 1)) == c.getSampleTimeStamp().toMicroseconds());
                counter++;
            }
            TimeStamp after;
            cout << "Duration = " << (after - before).toMicroseconds() << endl;

            bool exceptionCaught = false;
            try {
                const Container& c55 = p2.getNextContainerToBeSent();
                (void)c55;
            }
            catch(...) {
                exceptionCaught = true;
            }

            TS_ASSERT(exceptionCaught);

            UNLINK("PlayerModuleTest2.rec");
        }

        void notestCorrectMonotonicTemporalOrderAndRewindAndException() {
            cout << __FILE__ << " " << __LINE__ << endl;
            // Prepare record file.
            fstream fout("PlayerModule2Test2.rec", ios::out | ios::binary | ios::trunc);

            // Write data with non-monotonic order.
            TimeStamp ts1(4, 5);
            TimeStamp ts2(2, 3);
            TimeStamp ts3(0, 1);
            TimeStamp ts4(1, 2);
            TimeStamp ts5(3, 4);

            Container c1(ts1);
            c1.setSampleTimeStamp(ts1);
            fout << c1;

            Container c2(ts2);
            c2.setSampleTimeStamp(ts2);
            fout << c2;

            Container c3(ts3);
            c3.setSampleTimeStamp(ts3);
            fout << c3;

            Container c4(ts4);
            c4.setSampleTimeStamp(ts4);
            fout << c4;

            Container c5(ts5);
            c5.setSampleTimeStamp(ts5);
            fout << c5;

            fout.flush();
            fout.close();

            const URL u("file://PlayerModule2Test2.rec");

            // Create Player2 instance.
            const bool NO_AUTO_REWIND = false;
            Player2 p2(u, NO_AUTO_REWIND);

            TimeStamp before;
            int64_t counter = 0;
            bool didRewind = false;
            while (p2.hasMoreData()) {
                const Container& c = p2.getNextContainerToBeSent();
                if (counter == 0) {
                    TS_ASSERT(p2.getDelay() == 0);
                }
                else {
                    TS_ASSERT(p2.getDelay() == 1000001);
                }

                TS_ASSERT((counter * 1000 * 1000 + (counter + 1)) == c.getSampleTimeStamp().toMicroseconds());
                counter++;
                if (!didRewind && (counter == 3)) {
                    p2.rewind();
                    counter = 0;
                    didRewind = true;
                }
            }
            TimeStamp after;
            cout << "Duration = " << (after - before).toMicroseconds() << endl;

            bool exceptionCaught = false;
            try {
                const Container& c55 = p2.getNextContainerToBeSent();
                (void)c55;
            }
            catch(...) {
                exceptionCaught = true;
            }

            TS_ASSERT(exceptionCaught);

            UNLINK("PlayerModule2Test2.rec");
        }

        void notestCorrectMonotonicTemporalOrderAndAutoRewind() {
            cout << __FILE__ << " " << __LINE__ << endl;
            // Prepare record file.
            fstream fout("PlayerModule2Test2.rec", ios::out | ios::binary | ios::trunc);

            // Write data with non-monotonic order.
            TimeStamp ts1(4, 5);
            TimeStamp ts2(2, 3);
            TimeStamp ts3(0, 1);
            TimeStamp ts4(1, 2);
            TimeStamp ts5(3, 4);

            Container c1(ts1);
            c1.setSampleTimeStamp(ts1);
            fout << c1;

            Container c2(ts2);
            c2.setSampleTimeStamp(ts2);
            fout << c2;

            Container c3(ts3);
            c3.setSampleTimeStamp(ts3);
            fout << c3;

            Container c4(ts4);
            c4.setSampleTimeStamp(ts4);
            fout << c4;

            Container c5(ts5);
            c5.setSampleTimeStamp(ts5);
            fout << c5;

            fout.flush();
            fout.close();

            const URL u("file://PlayerModule2Test2.rec");

            // Create Player2 instance.
            const bool AUTO_REWIND = true;
            Player2 p2(u, AUTO_REWIND);

            TimeStamp before;
            int64_t counter = 0;
            uint32_t containerCounter = 0;
            bool didAutoRewind = false;
            while (p2.hasMoreData()) {
                const Container& c = p2.getNextContainerToBeSent();
                if (counter == 0) {
                    TS_ASSERT(p2.getDelay() == 0);
                }
                else {
                    TS_ASSERT(p2.getDelay() == 1000001);
                }

                TS_ASSERT((counter * 1000 * 1000 + (counter + 1)) == c.getSampleTimeStamp().toMicroseconds());
                counter++;
                if (counter == 5) {
                    counter = 0;
                }

                containerCounter++;
                if (containerCounter == 10) {
                    didAutoRewind = true;
                    break;
                }
            }
            TimeStamp after;
            cout << "Duration = " << (after - before).toMicroseconds() << endl;

            TS_ASSERT(didAutoRewind);

            bool noExceptionThrown = true;
            try {
                const Container& c55 = p2.getNextContainerToBeSent();
                (void)c55;
            }
            catch(...) {
                noExceptionThrown = false;
            }

            TS_ASSERT(noExceptionThrown);

            UNLINK("PlayerModule2Test2.rec");
        }

//        void notestLargeFileTwiddle() {
//            cout << __FILE__ << " " << __LINE__ << endl;
//            const URL u("file://test.rec");

//            // Create Player2 instance.
//            const bool NO_AUTO_REWIND = false;
//            Player2 p2(u, NO_AUTO_REWIND);

//            vector<float> p(3);
//            p[0] = 1;
//            p[1] = 1;
//            p[2] = 0;

//            vector<float> dp(3);
//            dp[0] = 1;
//            dp[1] = 1;
//            dp[2] = 1;

//            float err = 0;
//            float best_err = numeric_limits<float>::max();
//            float threshold = 0.001;

//            while (fabs(dp[0] + dp[1]) > threshold) {
//                for(int i = 0; i < 2; i++) {
//                    p[i] += dp[i];

//                    // Compute the results.
//                    p2.setPID(p[0], p[1], p[2]);
//                    p2.rewind();
//                    while (p2.hasMoreData()) {
//                        const Container& c = p2.getNextContainerToBeSent();
//                        (void)c;
//                        Thread::usleepFor(p2.getDelay()*0.02);
//                    }
//                    // Get error.
//                    err = p2.m_err;
//cout << __LINE__ << ": " << err << endl;
// 
//                    if (err < best_err) {
//                        // Improvement.
//                        best_err = err;
//                        dp[i] *= 1.1;
//                    }
//                    else {
//                        // No improvement.
//                        p[i] -= 2*dp[i]; // Go to the other direction.

//                        // Compute the results.
//                        p2.setPID(p[0], p[1], p[2]);
//                        p2.rewind();
//                        while (p2.hasMoreData()) {
//                            const Container& c = p2.getNextContainerToBeSent();
//                            (void)c;
//                            Thread::usleepFor(p2.getDelay()*0.02);
//                        }
//                        // Get error.
//                        err = p2.m_err;
//cout << __LINE__ << ": " << err << endl;

//                        if (err < best_err) {
//                            // Improvement.
//                            best_err = err;
//                            dp[i] *= 1.05;
//                        }
//                        else {
//                            // No improvement.
//                            p[i] += dp[i]; // As there was no improvement, the step size in either direction might be too big.
//                            dp[i] *= 0.95;
//                        }
//                    }
//cout << "best_err = " << best_err << ", P = " << p[0] << ", I = " << p[1] << ", D = " << p[2] << endl;
//cout << endl;
//                }
//            }
//        }

        void testLargeFile() {
            cout << __FILE__ << " " << __LINE__ << endl;
            const URL u("file://CID-251-odrecorderh264_2017-02-10_10:16:17.rec");

            // Create Player2 instance.
            const bool NO_AUTO_REWIND = false;
            Player2 p2(u, NO_AUTO_REWIND);

            TimeStamp before;
            int64_t counter = 0;
            while (p2.hasMoreData()) {
                const Container& c = p2.getNextContainerToBeSent();
                (void)c;
                counter++;

Thread::usleepFor(p2.getDelay()*0.02);
            }
            TimeStamp after;
            cout << "Found " << counter << " containers. Duration = " << static_cast<uint64_t>((after - before).toMicroseconds()) << endl;
        }
};

#endif /*PLAYER2TESTSUITE_H_*/