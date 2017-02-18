/*!
 *	@file	BasePluginEngine.h
 *	@brief	Provides the basis of a generic plugin engine.
 */

#ifndef __BASE_PLUGIN_ENGINE_H__
#define __BASE_PLUGIN_ENGINE_H__

#include "MengeCore/Runtime/Logger.h"
#include "MengeCore/Runtime/os.h"

#include <map>

namespace Menge {
	namespace PluginEngine {
		template <typename EngineType, typename PluginType>
		class BasePluginEngine {
		public:

			/*!
			 *	@brief		Mapping from the file name of the dynamic library to a pointer to
			 *				the loaded plugin.
			 */
			typedef std::map< std::string, PluginType * > PluginMap;

			/*!
			 *	@brief		Default constructor.
			 */
			BasePluginEngine() : _plugins() {}

			/*!
			 *	@brief		Virtual destructor.
			 */
			virtual ~BasePluginEngine() {}

			/*!
			 *	@brief		Initialize plug-ins from the given directory.
			 *
			 *	@param		pluginFolder		The folder to search for plugins.
			 *	@returns	The number of plugins successfully loaded.
			 */
			size_t loadPlugins( const std::string & pluginFolder ) {
				//	1. get all files in pluginFolder that conform to a platform-dependent
				//		naming convention.
				//	2. For each one, try to open it and determine if it has the interface
				//		expected for this plugin engine
				//	3. If it does, register it with the appropriate manager.
				//			(NOt entirely sure what this means yet...ultimately, the manager
				//			has to be able to provide information about the entity in the
				//			plug-in and provide the ability to insantiate it.  "information"
				//			may also include the ability to parse itself from the xml.

				StringList files;
#ifdef _MSC_VER
				std::string extension( "*.dll" );
#else
				std::string extension( "*.so" );
#endif
				if ( !os::listdir( pluginFolder, files, extension ) ) {
					return 0;
				}

				logger << Logger::INFO_MSG << getIntroMessage();

				StringListCItr itr = files.begin();
				for ( ; itr != files.end(); ++itr ) {
					std::string fullPath;
					if ( !os::path::absPath( os::path::join( 2, pluginFolder.c_str(), ( *itr ).c_str() ),
						fullPath ) ) {
						logger << Logger::ERR_MSG << "Unable to get absolute path for " << ( *itr );
						continue;
					}
					PluginType * plugin;
					try {
						plugin = new PluginType( fullPath );
					} catch ( std::exception & ) {
						logger << Logger::ERR_MSG << "Failed loading " << fullPath << " as a plug-in";
						continue;
					}
					logger << Logger::INFO_MSG << "Loaded: " << plugin->getName() << "\n";;
					logger << "\t" << plugin->getDescription();
					plugin->registerPlugin( static_cast<EngineType *>(this) );
					_plugins.insert( typename PluginMap::value_type( ( *itr ), plugin ) );
				}

				return files.size();
			}

		protected:
			/*!
			 *	@brief		Provides the string that will be written to the logger as an info
			 *				at the beginning of plugin loading.  It should be specialized for each
			 *				type of plugin engine.
			 */
			virtual std::string getIntroMessage() = 0;

			/*!
			 *	@brief		The loaded plugins.
			 */
			PluginMap	_plugins;
		};
	}
}

#endif	// __BASE_PLUGIN_ENGINE_H__