#pragma once

#include<iostream>

enum class VTCPOpcodes : std::int8_t {
    ON_VTCP_CONNECT_REQUEST,
    ON_VTCP_DISCONNECT_REQUEST,
    ON_VTCP_TEST_REQUEST,
    ON_VTCP_SSP_REQUEST
};