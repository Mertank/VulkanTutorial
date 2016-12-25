#ifndef __VKWRAPPER_H__
#define __VKWRAPPER_H__

#include <vulkan\vulkan.h>
#include <functional>

namespace tut {

template<typename T>
class VKWrapper {
public:
	/*
	===============
	VKWrapper::VKWrapper

		Default no-arg constructor
	===============
	*/
	VKWrapper( void ) :
		VKWrapper( []( T, VkAllocationCallbacks* ) {} )
	{}
	/*
	===============
	VKWrapper::VKWrapper

		Constructor by passing in the function pointer to delete the m_object
	===============
	*/
	VKWrapper( std::function<void( T, VkAllocationCallbacks* )> deletef ) {
		deleter = [ = ]( T obj ) { deletef( obj, nullptr ); };
	}
	/*
	===============
	VKWrapper::VKWrapper

		Constructor for a VkInstance
	===============
	*/
	VKWrapper( const VKWrapper<VkInstance>& instance, std::function<void( VkInstance, T, VkAllocationCallbacks* )> deletef ) {
		deleter = [ &instance, deletef ]( T obj ) { deletef( instance, obj, nullptr ); };
	}
	/*
	===============
	VKWrapper::VKWrapper

		Constructor for a VkDevice
	===============
	*/
	VKWrapper( const VKWrapper<VkDevice>& device, std::function<void( VkDevice, T, VkAllocationCallbacks* )> deletef ) {
		deleter = [ &device, deletef ]( T obj ) { deletef( device, obj, nullptr ); };
	}
	/*
	===============
	VKWrapper::~VKWrapper

		Destructor for the VKWrapper. Cleans up resources
	===============
	*/
	VKWrapper::~VKWrapper( void ) {
		cleanup();
	}

	/*
	===============
	VKWrapper::operator&

		Returns the m_object this wrapper is holding
	===============
	*/
	const T* operator&( void ) const {
		return &m_object;
	}
	/*
	===============
	VKWrapper::operator 

		Casts this wrapper to it's source m_object
	===============
	*/
	operator T( void ) const {
		return m_object;
	}
	/*
	===============
	VKWrapper::operator=

		Assignment operator cleans up current reference first
	===============
	*/
	void operator=( T rhs ) {
		if ( rhs != m_object ) { //Don't do this if assigning to self
			cleanup();
			m_object = rhs;
		}
	}

	/*
	===============
	VKWrapper::operator==

		Compares this m_object to an m_object of another type
	===============
	*/
	template<typename V>
	bool operator==( V rhs ) const {
		return m_object == ( T )rhs;
	}	

	/*
	===============
	VKWrapper::replace

	Frees up the current m_object before returning a reference to it.
	===============
	*/
	T* replace() {
		cleanup();
		return &m_object;
	}

private:
	T							m_object{ VK_NULL_HANDLE };
	std::function<void( T )>	deleter;

	/*
	===============
	VKWrapper::cleanup

		Frees up the current m_object's resource from Vulkan
	===============
	*/
	void cleanup( void ) {
		if ( m_object != VK_NULL_HANDLE ) {
			deleter( m_object );
		}

		m_object = VK_NULL_HANDLE;
	}
};

}

#endif //__VKWRAPPER_H__