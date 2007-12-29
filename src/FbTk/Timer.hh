// Timer.hh for FbTk - Fluxbox Toolkit
// Copyright (c) 2002-2003 Henrik Kinnunen (fluxgen at fluxbox dot org)
//
// Timer.hh for Blackbox - An X11 Window Manager
// Copyright (c) 1997 - 2000 Brad Hughes (bhughes at tcac.net)
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software. 
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#ifndef FBTK_TIMER_HH
#define FBTK_TIMER_HH

#include "RefCount.hh"
#include "Command.hh"

#ifdef HAVE_CTIME
  #include <ctime>
#else
  #include <time.h>
#endif
#include <list>
#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif //HAVE_CONFIG_H

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif // HAVE_INTTYPES_H

#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

namespace FbTk {

/**
    Handles Timeout
*/
class Timer {
public:
    Timer();
    explicit Timer(RefCount<Command> &handler);
    virtual ~Timer();

    void fireOnce(bool once) { m_once = once; }
    /// set timeout
    void setTimeout(time_t val);
    /// set timeout 
    void setTimeout(const timeval &val);
    void setCommand(RefCount<Command> &cmd);
    void setInterval(int val) { m_interval = val; }
    /// start timing
    void start();
    /// stop timing
    void stop();
    /// update all timers
    static void updateTimers(int file_descriptor);

    int isTiming() const { return m_timing; }
    int getInterval() const { return m_interval; }

    int doOnce() const { return m_once; }

    const timeval &getTimeout() const { return m_timeout; }
    const timeval &getStartTime() const { return m_start; }
    void makeEndTime(timeval &tm) const;

protected:
    /// force a timeout
    void fireTimeout();

private:
    /// add a timer to the static list
    static void addTimer(Timer *timer);
    /// remove a timer from the static list
    static void removeTimer(Timer *timer);

    typedef std::list<Timer *> TimerList;
    static TimerList m_timerlist; ///< list of all timers, sorted by next trigger time (start + timeout)

    RefCount<Command> m_handler; ///< what to do on a timeout

    bool m_timing; ///< clock running?
    bool m_once;  ///< do timeout only once?
    int m_interval; ///< Is an interval-only timer (e.g. clock)
    // note that intervals only take note of the seconds, not microseconds

    timeval m_start;    ///< start time
    timeval m_timeout; ///< time length
};

/// executes a command after a specified timeout
class DelayedCmd: public Command {
public:
    DelayedCmd(RefCount<Command> &cmd, unsigned int timeout = 200000);
    void execute();
    static Command *parse(const std::string &command,
                          const std::string &args, bool trusted);
private:
    Timer m_timer;
};

} // end namespace FbTk

#endif // FBTK_TIMER_HH
