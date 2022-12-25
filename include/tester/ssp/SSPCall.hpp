#pragma once

#include<iostream>
#include<pjsua2.hpp>

#include"SSPAccount.hpp"

class SSPCall : public pj::Call
{
public:
    SSPCall(pj::Account &acc, int call_id = PJSUA_INVALID_ID)
    : pj::Call(acc, call_id)
    {
    	_wav_player = NULL;
        _myAcc = (SSPAccount *)&acc;
    }
    
    ~SSPCall()
    {
    	if (_wav_player)
    	    delete _wav_player;
    }

    void onCallState(pj::OnCallStateParam &prm);
    void onCallMediaState(pj::OnCallMediaStateParam &prm);
    void onCallTransferRequest(pj::OnCallTransferRequestParam &prm);
    void onCallReplaceRequest(pj::OnCallReplaceRequestParam &prm);
    
    void callTo(std::string & destination_uri);

private:
    SSPAccount *_myAcc;
    pj::AudioMediaPlayer *_wav_player;
};

    void SSPCall::onCallState(pj::OnCallStateParam &prm)
    {
        PJ_UNUSED_ARG(prm);

        pj::CallInfo ci = getInfo();
        std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
                << "]" << std::endl;
        //TODO add more states
        if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
            //_myAcc->removeCall(this);
            /* Delete the call */
            //delete this;
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


    void SSPCall::onCallTransferRequest(pj::OnCallTransferRequestParam &prm)
    {
        /* Create new Call for call transfer */
        prm.newCall = new SSPCall(*_myAcc);
    }

    void SSPCall::onCallReplaceRequest(pj::OnCallReplaceRequestParam &prm)
    {
        /* Create new Call for call replace */
        prm.newCall = new SSPCall(*_myAcc);
    }

    void SSPCall::callTo(std::string & destination_uri)
    {
        pj::CallOpParam prm(true);
        prm.opt.audioCount = 1;
        prm.opt.videoCount = 0;
        makeCall(destination_uri, prm);
    }