#pragma once

enum class VTCPOpcode : int {
    VTCP_CONNECT_REQ = 1,
    VTCP_CONNECT_RES = 2,

    VTCP_DISCONNECT_REQ = 3,
    VTCP_DISCONNECT_RES = 4,

    VTCP_AUTO_TEST_REQ = 5,
    VTCP_AUTO_TEST_RES = 6,

    VTCP_HISTORY_HEADER_REQ = 7,
    VTCP_HISTORY_HEADER_RES = 8,
    
    VTCP_HISTORY_LOG_REQ = 9,
    VTCP_HISTORY_LOG_RES = 10
};