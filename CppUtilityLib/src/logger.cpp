#include "logger.h"

FILE *StreamOutput::logStream_mp = stdout;

const char *logLevelText[9] = {
    "EMERGENCY:",
    "    ALERT:",
    " CRITICAL:",
    "   ERROR: ",
    " WARNING: ",
    "notice: ",
    "  info: ",
    " debug: ",
    " trace: "
};

template <typename OutputPolicy>
logLevel Logger<OutputPolicy>::reportingLevel_m = LM_INFO;

template <typename OutputPolicy>
std::ostringstream& Logger<OutputPolicy>::get(logLevel level, const Time *TS) {
    // Generate a timestamp if needed:
    if (!TS) {
        Time timestamp;
        setTimeStamp(&timestamp);
        TS = &timestamp;
    } 
    // Write the time stamp and logging level to the buffer:
    std::string ts;
    os << timestampToText(TS, ts) << logLevelText[level];
    // Return the stringstream so that the caller can append stuff:
    return os;
}

//explicit instantiation of the logger methods:
template class Logger<StreamOutput>;




