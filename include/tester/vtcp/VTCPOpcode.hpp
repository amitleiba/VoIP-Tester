#pragma once

enum class VTCPOpcode : int {
    ON_VTCP_CONNECT_REQUEST = 1,
    ON_VTCP_DISCONNECT_REQUEST = 2,
    ON_VTCP_TEST_REQUEST = 3,
    ON_VTCP_SSP_REQUEST = 4
};