#ifndef NOTIFICATION_GLUE_H
#define NOTIFICATION_GLUE_H

#include <stdint.h>

#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

/**
 * @brief notification callback
 *
 * @par    id      = id of the notification
 * @par    action  = index of the clicked button or reason for dismissal or 0 for failed callback
 * @return return value is ignored it is used only for compatability
**/
typedef uint64_t(*callback_func)(uint64_t id, int action);

/**
 * @brief Initialize notifications
 *
 * @par    appName      = application name, appears in the notification header
 * @par    company      = company name used for notification metadata
 * @par    productName  = product name used for notification metadata
 * @par    subProduct   = sub product name used for notification metadata
 * @par    versionInfo  = version info used for notification metadata
 * @return 1 on success 0 on failure 
 *
 */
EXPORT uint64_t PortmasterToastInitialize(const wchar_t* appName, const wchar_t* company, const wchar_t* productName, const wchar_t* subProduct, const wchar_t* versionInfo);

/**
 * @brief check if notifications are initialized
 * @return 1 for true 0 for false
 * 
 */
EXPORT uint64_t PortmasterToastIsInitialized();

/**
 * @brief crates a notification object
 * @par    title      = title of the notification
 * @par    content    = text content of the notification
 * @return pointer to the notification object
 * @note PortmasterToastDeleteNotification must be called always after create or there will be memory leak 
 */
EXPORT void* PortmasterToastCreateNotification(const wchar_t* title, const wchar_t* content);


/**
 * @brief deletes a notification object
 * @par    notification = pointer to notification
 * @note Must be called on PortmasterToastCreateNotification return value
 */
EXPORT void PortmasterToastDeleteNotification(void *notification);

/**
 * @brief adds a button to the notification
 * @par    notification = pointer to a notification object
 * @par    buttonText   = text of the button that is going to be added
 * @return 1 for success 0 for failure
 */
EXPORT uint64_t PortmasterToastAddButton(void *notification, wchar_t *buttonText);

/**
 * @brief sets the notification icon
 * @par    notification = pointer to a notification object
 * @par    imagePath   = path to the image file
 * @return 1 for success 0 for failure
 */
EXPORT uint64_t PortmasterToastSetImage(void *notification, wchar_t *imagePath);

/**
 * @brief make a request to the OS to show the notification
 * @par    notification = pointer to a notification object
 * @return Id of the notification or -1 for failure
 */
EXPORT uint64_t PortmasterToastShow(void *notification);

/**
 * @brief hides previously shown notification
 * @par    notification = pointer to a notification object
 * @return 1 for success 0 for failure
 */
EXPORT uint64_t PortmasterToastHide(uint64_t notificationID);

/**
 * @brief set callback function that well be called when notification button is clicked
 *		  Or if the notification is clicked. In that case the action id will be -1 
 * @par    func = pointer to a valid function see callback_func type
 * @return 1 for success 0 for failure
 */
EXPORT uint64_t PortmasterToastActivatedCallback(callback_func func);

/**
 * @brief set callback function that well be called when notification is dismissed
 *			0 -> UserCanceled 
 *			1 -> ApplicationHidden
 *			2 -> TimedOut
 * @par    func = pointer to a valid function see callback_func type
 * @return 1 for success 0 for failure
 */
EXPORT uint64_t PortmasterToastDismissedCallback(callback_func func);

/**
 * @brief set callback function that well be called if the notification failed
 *		   
 * @par    func = pointer to a valid function see callback_func type
 * @return 1 for success 0 for failure
 */
EXPORT uint64_t PortmasterToastFailedCallback(callback_func func);


#endif // NOTIFICATION_GLUE_H