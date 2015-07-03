# uac_no_securedesktop

Sample code to temporarily show Windows UAC prompts on normal instead of secure desktop for a single session

UAC prompts are normally displayed on the secure desktop, which is accessible only as LocalSystem. There are registry keys / security policies that disable this, but modifying them affects the entire machine.

Microsoft Remote Assistance uses undocumented features to temporarily show the UAC prompts on the normal desktop, affecting only the current session, and safely reverting the change when no longer needed.

An out-of-process local server COM object is used to communicate with AIS in order to use this feature. The COM object is hosted in sdchange.exe via the coclass SDChangeObj, and uses the interface ISDChangeObj which has a single function, UseSecureDesktop(long);

This sample shows how you can use this feature, and use ::CoGetObject with an elevation moniker to allow it to be used even from a non-elevated process. Of course, an elevation prompt will still occur for the external process.

As this is undocumented, there is no guarantee that this will work in future versions of Windows, but as of now it works from Windows Vista through Windows 10.