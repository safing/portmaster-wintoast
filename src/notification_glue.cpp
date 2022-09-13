#include "notification_glue.h"
#include "wintoastlib.h"

using namespace WinToastLib;

static callback_func activatedCallback = NULL;

class WinToastHandler : public IWinToastHandler
{
public:
    WinToastHandler() {}

    void toastActivated() const override {}
    void toastActivated(int actionIndex) const override {
        if(activatedCallback != NULL) {
            // Calling go function
            activatedCallback(m_id, actionIndex);
        }
    }
    void toastDismissed(WinToastDismissalReason state) const override {}
    void toastFailed() const override {}

    void setID(uint64_t id) {
        m_id = id;
    }
private:
    uint64_t m_id;
};

uint64_t PortmasterToastInitialize(const wchar_t *appName, const wchar_t *company, const wchar_t *productName, const wchar_t *subProduct, const wchar_t *versionInfo) {
    WinToast::instance()->setAppName(appName);
    const auto aumi = WinToast::configureAUMI(company, productName, subProduct, versionInfo);
    WinToast::instance()->setAppUserModelId(aumi);

    WinToast::WinToastError error;
    if (!WinToast::instance()->initialize(&error)) {
        return 0;
    }

    return 1;
}

uint64_t PortmasterToastIsInitialized() {
    return (uint64_t) WinToast::instance()->isInitialized();
}

void* PortmasterToastCreateNotification(const wchar_t *title, const wchar_t *content) {
    WinToastTemplate *templ = new WinToastTemplate(WinToastTemplate::ImageAndText02);
    
    templ->setTextField(title, WinToastTemplate::FirstLine);
    templ->setTextField(content, WinToastTemplate::SecondLine);

    templ->setDuration(WinToastTemplate::Duration::Long);
    templ->setAudioOption(WinToastTemplate::AudioOption::Default);
    templ->setAudioPath(WinToastTemplate::AudioSystemFile::Call1);

    return templ;
}

uint64_t PortmasterToastAddButton(void *notification, wchar_t *buttonText) {
    if(notification == NULL || buttonText == NULL) {
        return 0;
    }

    WinToastTemplate *winToastPtr = (WinToastTemplate*) notification;
    winToastPtr->addAction(buttonText);
    return 1;
}

uint64_t PortmasterToastSetImage(void *notification, wchar_t *imagePath) {
    if(notification == NULL || imagePath == NULL) {
        return 0;
    }

    WinToastTemplate *winToastPtr = (WinToastTemplate*) notification;
    winToastPtr->setImagePath(imagePath);
    return 1;
}

uint64_t PortmasterToastShow(void *notification) {
    if(notification == NULL) {
        return -1;
    }
    WinToastTemplate *winToastPtr = (WinToastTemplate*) notification;
    
    auto handler = std::make_shared<WinToastHandler>();
    int64_t toastID = WinToast::instance()->showToast(*winToastPtr, handler);
    handler->setID(toastID);
    delete winToastPtr;
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
    if(func == NULL) {
        return 0;
    }
    
    activatedCallback = func;
    return 1;
}