#pragma once

enum class VTCPOpcodes : int {
    onVtcpConnectRequest = 1,
    onVtcpDisconnectRequest = 2,
    onVtcpTestRequest = 3,
    onVtcpSspRequest = 4
};