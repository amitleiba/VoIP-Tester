#include "tester/ssp/SSPCall.hpp"

#include "tester/ssp/SSPAccount.hpp"

SSPCall::SSPCall(pj::Account &acc, std::function<void(const pj::OnCallStateParam & )> onCallState,
    int call_id): pj::Call(acc, call_id), _onCallState(std::move(onCallState))
{
    
}

SSPCall::~SSPCall()
{
    LOG_INFO << "Call was deleted" << std::endl;
    std::cout << "Call was deleted" << std::endl;
}

void SSPCall::onCallState(pj::OnCallStateParam &prm)
{
    _onCallState(prm);
    if (getInfo().state == PJSIP_INV_STATE_DISCONNECTED)
    {
        delete this;
    }
}

void SSPCall::onCallMediaState(pj::OnCallMediaStateParam &prm)
{
    pj::CallInfo ci = getInfo();
    // Iterate all the call medias
    for (unsigned i = 0; i < ci.media.size(); i++) {
        if (ci.media[i].type==PJMEDIA_TYPE_AUDIO && getMedia(i)) {
            pj::AudioMedia *aud_med = (pj::AudioMedia *)getMedia(i);

            // Connect the call audio media to sound device
            pj::AudDevManager& mgr = pj::Endpoint::instance().audDevManager();
            aud_med->startTransmit(mgr.getPlaybackDevMedia());
            mgr.getCaptureDevMedia().startTransmit(*aud_med);
        }
    }
}