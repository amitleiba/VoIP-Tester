#pragma once

enum class VTCPOpcode : int {
    VTCP_CONNECT = 1,
    VTCP_DISCONNECT = 2,
    VTCP_AUTO_TEST = 3,
    VTCP_MANUAL_TEST = 4
};