#pragma once

#include<iostream>
#include<functional>

#include<pjsua2.hpp>

class SSPCall : public pj::Call
{
public:
    SSPCall(pj::Account *acc, std::function<void(pj::CallInfo ci,const pj::OnCallStateParam & prm)> onCallState,
        int call_id = PJSUA_INVALID_ID): pj::Call(*acc, call_id), _onCallState(std::move(onCallState))
    {}

    ~SSPCall() = default;

    void onCallState(pj::OnCallStateParam &prm) override
    {
        pj::CallInfo ci = getInfo();
        _onCallState(std::move(ci), prm);
    }

    void onCallMediaState(pj::OnCallMediaStateParam &prm) override
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
    
    void callTo(const std::string & destination_uri)
    {
        pj::CallOpParam prm(true);
        prm.opt.audioCount = 1;
        prm.opt.videoCount = 0;
        makeCall(destination_uri, prm);
    }

private:
    std::function<void(pj::CallInfo,const pj::OnCallStateParam &)> _onCallState;
};