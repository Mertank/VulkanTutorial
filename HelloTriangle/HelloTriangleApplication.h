#ifndef __HELLOTRIANGLEAPPLICATION_H__
#define __HELLOTRIANGLEAPPLICATION_H__

namespace tut {

class HelloTriangleApplication {
public:
	void run( void );

private:
	void initVulkan( void );
	void mainLoop( void );
};

}

#endif // !__HELLOTRIANGLEAPPLICATION_H__

