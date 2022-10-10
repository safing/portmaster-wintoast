#include "notification_glue.h"
#include "wintoastlib.h"

using namespace WinToastLib;

static callback_func activatedCallback = nullptr;
static callback_func dissmisedCallback = nullptr;
static callback_func failedCallback = nullptr;

class WinToastHandler : public IWinToastHandler
{
public:
    WinToastHandler() {}

    void toastActivated() const override {
        if (activatedCallback != nullptr) {
            // Calling go function
            activatedCallback(m_id, -1);
        }
    }
    void toastActivated(int actionIndex) const override {
        if(activatedCallback != nullptr) {
            // Calling go function
            activatedCallback(m_id, actionIndex);
        }
    }
    void toastDismissed(WinToastDismissalReason state) const override {
        if (dissmisedCallback != nullptr) {
            // Calling go function
            dissmisedCallback(m_id, state);
        }
    }
    void toastFailed() const override {
        if (failedCallback != nullptr) {
            // Calling go function
            failedCallback(m_id, 0);
        }
    }

    void setID(uint64_t id) {
        m_id = id;
    }
private:
    uint64_t m_id;
};

uint64_t PortmasterToastInitialize(const wchar_t *appName, const wchar_t *aumi) {
    WinToast::instance()->setAppName(appName);
    WinToast::instance()->setAppUserModelId(aumi);
    WinToast::instance()->setShortcutPolicy(WinToast::SHORTCUT_POLICY_IGNORE);

    WinToast::WinToastError error = WinToast::NoError;
    WinToast::instance()->initialize(&error);
    return error;
}

uint64_t PortmasterToastIsInitialized() {
    return (uint64_t) WinToast::instance()->isInitialized();
}

void* PortmasterToastCreateNotification(const wchar_t *title, const wchar_t *content) {
    WinToastTemplate *templ = new WinToastTemplate(WinToastTemplate::ImageAndText02);
    
    templ->setTextField(title, WinToastTemplate::FirstLine);
    templ->setTextField(content, WinToastTemplate::SecondLine);

    templ->setDuration(WinToastTemplate::Duration::Long);

    return templ;
}

void PortmasterToastDeleteNotification(void* notification) {
    WinToastTemplate *winToastPtr = (WinToastTemplate*)notification;
    delete winToastPtr;
}

uint64_t PortmasterToastAddButton(void *notification, wchar_t *buttonText) {
    if(notification == nullptr || buttonText == nullptr) {
        return 0;
    }

    WinToastTemplate *winToastPtr = (WinToastTemplate*) notification;
    winToastPtr->addAction(buttonText);
    return 1;
}

uint64_t PortmasterToastSetImage(void *notification, wchar_t *imagePath) {
    if(notification == nullptr || imagePath == nullptr) {
        return 0;
    }

    WinToastTemplate *winToastPtr = (WinToastTemplate*) notification;
    winToastPtr->setImagePath(imagePath);
    return 1;
}

uint64_t PortmasterToastSetSound(void *notification, int option, int file) {
    if(notification == nullptr) {
        return 0;
    }
    WinToastTemplate *winToastPtr = (WinToastTemplate*) notification;
    winToastPtr->setAudioOption((WinToastTemplate::AudioOption) option);
    winToastPtr->setAudioPath((WinToastTemplate::AudioSystemFile) file);
    return 1;
}

uint64_t PortmasterToastShow(void *notification) {
    if(notification == nullptr) {
        return -1;
    }

    WinToastTemplate *winToastPtr = (WinToastTemplate*) notification;
    auto handler = std::make_shared<WinToastHandler>();
    int64_t toastID = WinToast::instance()->showToast(*winToastPtr, handler);

    handler->setID(toastID);
    return toastID; // -1 for error
}

uint64_t PortmasterToastHide(uint64_t notificationID) {
    bool success = WinToast::instance()->hideToast(notificationID);
    if(!success) {
        return 0;
    }

    return 1;
}

uint64_t PortmasterToastActivatedCallback(callback_func func) {
    if(func == nullptr) {
        return 0;
    }
    
    activatedCallback = func;
    return 1;
}

uint64_t PortmasterToastDismissedCallback(callback_func func) {
    if (func == nullptr) {
        return 0;
    }

    dissmisedCallback = func;
    return 1;
}

uint64_t PortmasterToastFailedCallback(callback_func func) {
    if (func == nullptr) {
        return 0;
    }

    failedCallback = func;
    return 1;
}
