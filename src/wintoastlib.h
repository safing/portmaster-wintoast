/* * Copyright (C) 2016-2019 Mohammed Boujemaoui <mohabouje@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef WINTOASTLIB_H
#define WINTOASTLIB_H
#include <Windows.h>
#include <sdkddkver.h>
#include <WinUser.h>
#include <ShObjIdl.h>
#include <wrl.h>
#include <wrl/implements.h>
#include <wrl/event.h>
#include <windows.ui.notifications.h>
#include <strsafe.h>
#include <Psapi.h>
#include <ShlObj.h>
#include <roapi.h>
#include <propvarutil.h>
#include <functiondiscoverykeys.h>
#include <iostream>
#include <memory>
#include <winstring.h>
#include <string.h>
#include <vector>
#include <map>
using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Notifications;
using namespace Windows::Foundation;

// unique class ID for the application
#define NOTIFICATION_CALLBACK_CLSID "7F00FB48-65D5-4BA8-A35B-F194DA7E1A51"

namespace WinToastLib {

    class IWinToastHandler {
    public:
        enum WinToastDismissalReason {
            UserCanceled = ToastDismissalReason::ToastDismissalReason_UserCanceled,
            ApplicationHidden = ToastDismissalReason::ToastDismissalReason_ApplicationHidden,
            TimedOut = ToastDismissalReason::ToastDismissalReason_TimedOut
        };
        virtual ~IWinToastHandler() = default;
        virtual void toastActivated() const = 0;
        virtual void toastActivated(int actionIndex) const = 0;
        virtual void toastDismissed(WinToastDismissalReason state) const = 0;
        virtual void toastFailed() const = 0;
    };

    class WinToastTemplate {
    public:
        enum class Scenario { Default, Alarm, IncomingCall, Reminder };
        enum Duration { System, Short, Long };
        enum AudioOption { Default = 0, Silent, Loop };
        enum TextField { FirstLine = 0, SecondLine, ThirdLine };
        enum WinToastTemplateType {
            ImageAndText01 = ToastTemplateType::ToastTemplateType_ToastImageAndText01, // Number of fields 1
            ImageAndText02 = ToastTemplateType::ToastTemplateType_ToastImageAndText02, // Number of fields 2
            ImageAndText03 = ToastTemplateType::ToastTemplateType_ToastImageAndText03, // Number of fields 2
            ImageAndText04 = ToastTemplateType::ToastTemplateType_ToastImageAndText04, // Number of fields 3
            Text01 = ToastTemplateType::ToastTemplateType_ToastText01, // Number of fields 1
            Text02 = ToastTemplateType::ToastTemplateType_ToastText02, // Number of fields 2
            Text03 = ToastTemplateType::ToastTemplateType_ToastText03, // Number of fields 2
            Text04 = ToastTemplateType::ToastTemplateType_ToastText04, // Number of fields 3
        };

        enum AudioSystemFile {
            DefaultSound,
            IM,
            Mail,
            Reminder,
            SMS,
            Alarm,
            Alarm2,
            Alarm3,
            Alarm4,
            Alarm5,
            Alarm6,
            Alarm7,
            Alarm8,
            Alarm9,
            Alarm10,
            Call,
            Call1,
            Call2,
            Call3,
            Call4,
            Call5,
            Call6,
            Call7,
            Call8,
            Call9,
            Call10,
        };


        WinToastTemplate(_In_ WinToastTemplateType type = WinToastTemplateType::ImageAndText02);
        ~WinToastTemplate();

        void setFirstLine(_In_ const std::wstring& text);
        void setSecondLine(_In_ const std::wstring& text);
        void setThirdLine(_In_ const std::wstring& text);
        void setTextField(_In_ const std::wstring& txt, _In_ TextField pos);
        void setAttributionText(_In_ const std::wstring& attributionText);
        void setImagePath(_In_ const std::wstring& imgPath);
        void setAudioPath(_In_ WinToastTemplate::AudioSystemFile audio);
        void setAudioPath(_In_ const std::wstring& audioPath);
        void setAudioOption(_In_ WinToastTemplate::AudioOption audioOption);
        void setDuration(_In_ Duration duration);
        void setExpiration(_In_ INT64 millisecondsFromNow);
        void setScenario(_In_ Scenario scenario);
        void addAction(_In_ const std::wstring& label);

        std::size_t textFieldsCount() const;
        std::size_t actionsCount() const;
        bool hasImage() const;
        const std::vector<std::wstring>& textFields() const;
        const std::wstring& textField(_In_ TextField pos) const;
        const std::wstring& actionLabel(_In_ std::size_t pos) const;
        const std::wstring& imagePath() const;
        const std::wstring& audioPath() const;
        const std::wstring& attributionText() const;
        const std::wstring& scenario() const;
        INT64 expiration() const;
        WinToastTemplateType type() const;
        WinToastTemplate::AudioOption audioOption() const;
        Duration duration() const;
    private:
        std::vector<std::wstring>           m_textFields{};
        std::vector<std::wstring>           m_actions{};
        std::wstring                        m_imagePath{};
        std::wstring                        m_audioPath{};
        std::wstring                        m_attributionText{};
        std::wstring                        m_scenario{L"Default"};
        INT64                               m_expiration{0};
        AudioOption                         m_audioOption{WinToastTemplate::AudioOption::Default};
        WinToastTemplateType                m_type{WinToastTemplateType::Text01};
        Duration                            m_duration{Duration::System};
    };

    class WinToast {
    public:
        enum WinToastError {
            NoError = 0,
            NotInitialized,
            SystemNotSupported,
            ShellLinkNotCreated,
            InvalidAppUserModelID,
            InvalidParameters,
            InvalidHandler,
            NotDisplayed,
            UnknownError
        };

        enum ShortcutResult {
            SHORTCUT_UNCHANGED = 0,
            SHORTCUT_WAS_CHANGED = 1,
            SHORTCUT_WAS_CREATED = 2,

            SHORTCUT_MISSING_PARAMETERS = -1,
            SHORTCUT_INCOMPATIBLE_OS = -2,
            SHORTCUT_COM_INIT_FAILURE = -3,
            SHORTCUT_CREATE_FAILED = -4
        };

        enum ShortcutPolicy {
            /* Don't check, create, or modify a shortcut. */
            SHORTCUT_POLICY_IGNORE = 0,
            /* Require a shortcut with matching AUMI, don't create or modify an existing one. */
            SHORTCUT_POLICY_REQUIRE_NO_CREATE = 1,
            /* Require a shortcut with matching AUMI, create if missing, modify if not matching.
             * This is the default. */
            SHORTCUT_POLICY_REQUIRE_CREATE = 2,
        };

        WinToast(void);
        virtual ~WinToast();
        static WinToast* instance();
        static bool isCompatible();
        static bool isSupportingModernFeatures();
        static std::wstring configureAUMI(_In_ const std::wstring& companyName,
                                          _In_ const std::wstring& productName,
                                          _In_ const std::wstring& subProduct = std::wstring(),
                                          _In_ const std::wstring& versionInformation = std::wstring());
        static const std::wstring& strerror(_In_ WinToastError error);
        virtual bool initialize(_Out_opt_ WinToastError* error = nullptr);
        virtual bool isInitialized() const;
        virtual bool hideToast(_In_ INT64 id);
        virtual INT64 showToast(_In_ const WinToastTemplate &toast, _In_ std::shared_ptr<IWinToastHandler> handler, _Out_opt_ WinToastError *error = nullptr);
        virtual void clear();
        virtual enum ShortcutResult createShortcut();

        const std::wstring& appName() const;
        const std::wstring& appUserModelId() const;
        void setAppUserModelId(_In_ const std::wstring &aumi);
        void setAppName(_In_ const std::wstring &appName);
        void setShortcutPolicy(_In_ ShortcutPolicy policy);
        HRESULT validateShellLinkHelper(_Out_ bool& wasChanged);

    protected:
        bool                                            m_isInitialized{false};
        bool                                            m_hasCoInitialized{false};
        ShortcutPolicy                                  m_shortcutPolicy{SHORTCUT_POLICY_REQUIRE_CREATE};
        std::wstring                                    m_appName{};
        std::wstring                                    m_aumi{};
        std::map<INT64, ComPtr<IToastNotification>>     m_buffer{};

        HRESULT createShellLinkHelper();
        HRESULT setImageFieldHelper(_In_ IXmlDocument *xml, _In_ const std::wstring& path);
        HRESULT setAudioFieldHelper(_In_ IXmlDocument *xml, _In_ const std::wstring& path, _In_opt_ WinToastTemplate::AudioOption option = WinToastTemplate::AudioOption::Default);
        HRESULT setTextFieldHelper(_In_ IXmlDocument *xml, _In_ const std::wstring& text, _In_ UINT32 pos);
        HRESULT setAttributionTextFieldHelper(_In_ IXmlDocument *xml, _In_ const std::wstring& text);
        HRESULT addActionHelper(_In_ IXmlDocument *xml, _In_ const std::wstring& action, _In_ const std::wstring& arguments);
        HRESULT addDurationHelper(_In_ IXmlDocument *xml, _In_ const std::wstring& duration);
        HRESULT addScenarioHelper(_In_ IXmlDocument *xml, _In_ const std::wstring& scenario);
        ComPtr<IToastNotifier> notifier(_In_ bool *succeeded) const;
        void setError(_Out_opt_ WinToastError *error, _In_ WinToastError value);
    };
}

typedef struct {} NOTIFICATION_USER_INPUT_DATA;

MIDL_INTERFACE("53E31837-6600-4A81-9395-75CFFE746F94")
INotificationActivationCallback : public IUnknown
{
public:
   virtual HRESULT STDMETHODCALLTYPE Activate(
           __RPC__in_string LPCWSTR appUserModelId, __RPC__in_opt_string LPCWSTR invokedArgs,
           __RPC__in_ecount_full_opt(count) const NOTIFICATION_USER_INPUT_DATA * data,
           ULONG count) = 0;
};

// The UUID CLSID must be unique to your app. Create a new GUID if copying this code.
class DECLSPEC_UUID(NOTIFICATION_CALLBACK_CLSID) NotificationActivator WrlSealed WrlFinal
    : public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
    INotificationActivationCallback>
{
public:
   virtual HRESULT STDMETHODCALLTYPE Activate(
       _In_ LPCWSTR appUserModelId,
       _In_ LPCWSTR invokedArgs,
       _In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA * data,
       ULONG dataCount) override
   {
       // Not used
       return S_OK;
   }
};

// Flag class as COM creatable
CoCreatableClass(NotificationActivator);

#endif // WINTOASTLIB_H
