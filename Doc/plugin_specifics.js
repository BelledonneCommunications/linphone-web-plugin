/**
 * The FileManager object is used to perform operation on local and/or remote files.
 * @external FileManager
**/

/**
 * Create a FileTransfer object to handle the copy of a file.
 * @function external:FileManager#copy
 * @param {string} sourceUrl - The URL of the file to copy
 * @param {string} targetUrl - The URL where to copy the file
 * @param {object} callback - The javascript function to be called when the operation is finished or on error.
 * This function will receive a boolean value telling whether the operation has been successful as its first argument.
 * If the operation failed, it will receive an error string as its second parameter.
 * @returns {external:FileTransfer} The FileTransfer object to handle the copy.
**/

/**
 * Test the existence of a file.
 * @function external:FileManager#exists
 * @param {string} url - The URL of the file to check
 * @param {object} callback - The javascript function to be called when the operation is finished or on error.
 * This function will receive a boolean value telling whether the file exists as its first argument.
 * If there was an error, it will receive an error string as its second parameter.
**/

/**
 * Create a directory.
 * @function external:FileManager#mkdir
 * @param {string} url - The URL of the directory to create
 * @param {object} callback - The javascript function to be called when the operation is finished or on error.
 * This function will receive a boolean value telling whether the directory has been created as its first argument.
 * If there was an error, it will receive an error string as its second parameter.
**/

/**
 * Remove a file or a directory.
 * @function external:FileManager#remove
 * @param {string} url - The URL of the file or directory to remove
 * @param {object} callback - The javascript function to be called when the operation is finished or on error.
 * This function will receive a boolean value telling whether the file or directory has been removed as its first argument.
 * If there was an error, it will receive an error string as its second parameter.
**/



/**
 * The FileTransfer object is used to handle the copy of files. It is created by a call to {@link external:FileManager#copy}.
 * @external FileTransfer
**/

/**
 * Get the error message when a file transfer has failed.
 * @member {string} external:FileTransfer#error
 * @readonly
**/

/**
 * Tells whether a file transfer is finished.
 * @member {boolean} external:FileTransfer#done
 * @readonly
**/

/**
 * Get the number of bytes that have been received.
 * @member {number} external:FileTransfer#receivedBytes
 * @readonly
**/

/**
 * Get the total number of bytes of the file transfer.
 * @member {number} external:FileTransfer#totalBytes
 * @readonly
**/

/**
 * The URL of the source of the file transfer.
 * @member {string} external:FileTransfer#sourceUrl
 * @readonly
**/

/**
 * The URL of the target of the file transfer.
 * @member {string} external:FileTransfer#targetUrl
 * @readonly
**/

/**
 * Start the file transfer
 * @function external:FileTransfer#start
**/

/**
 * Cancel the file transfer
 * @function external:FileTransfer#cancel
**/



/**
 * Get the file manager.
 * @member {external:FileManager} external:LinphoneCore#fileManager
**/

/**
 * Enable/disable the iteration process of the LinphoneCore.
 * @member {boolean} external:LinphoneCore#iterateEnabled
**/

/**
 * Set the interval between two iterations of the LinphoneCore process (in milliseconds).
 * @member {number} external:LinphoneCore#iterateInterval
**/

/**
 * Set the javascript function to be used as the log handler.
 * This function will received the log level as its first parameter and the log message as its second parameter.
 * @member {object} external:LinphoneCore#logHandler
**/

/**
 * Value to be used to identify the instance of the plugin.
 * @member {string} external:LinphoneCore#magic
**/

/**
 * Gets the version of the plugin.
 * @member {string} external:LinphoneCore#pluginVersion
 * @readonly
**/

/**
 * Gets the version of the core.
 * @member {string} external:LinphoneCore#version
 * @readonly
**/


/**
 * Initializes and launches the execution of the LinphoneCore.
 * @function external:LinphoneCore#init
 * @param {string} config - Path to the user config file. If it does not exists it will be created.
 * The config file is used to store all settings, call logs, friends, proxies... so that all these settings become persistent over the life of the LinphoneCore object.
 * It is allowed to set a null config file. In that case LinphoneCore will not store any settings.
 * @param {string} factory - Path to the read-only factory config file. It can be used to store hard-coded preference such as proxy settings or internal preferences.
 * The settings in this factory file always override the one in the normal config file.
 * It is OPTIONAL, use null if unneeded.
 * @returns {number} 0 if successful, any other value on error
**/

/**
 * Stops the execution and uninitializes the LinphoneCore.
 * @function external:LinphoneCore#uninit
 * @return {number} 0 if successful, any other value on error
**/
