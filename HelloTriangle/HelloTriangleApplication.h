#ifndef __HELLOTRIANGLEAPPLICATION_H__
#define __HELLOTRIANGLEAPPLICATION_H__

struct GLFWwindow;

namespace tut {

class HelloTriangleApplication {
public:
	int			run( void );

private:
	void		initVulkan( void );
	void		initWindow( void );
	void		mainLoop( void );

	GLFWwindow*	m_window{ nullptr };

	const int	WIDTH { 800 };
	const int	HEIGHT{ 600 };
};

}

#endif // !__HELLOTRIANGLEAPPLICATION_H__

