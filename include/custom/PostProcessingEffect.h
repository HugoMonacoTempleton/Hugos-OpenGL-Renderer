#ifndef PP_EFFECT_H
#define PP_EFFECT_H

#include <custom/GBuffer.h>

/*---------------------------------------------------------------------*/
/* POST PROCESSING EFFECT: Abstract class for post processing effects. */
/*---------------------------------------------------------------------*/
class PostProcessingEffect {
public: 
	PostProcessingEffect() {

	}

	virtual void Render(GBuffer &FrameBuffer, int SCR_WIDTH, int SCR_HEIGHT) = 0;

	virtual void Menu(int pos) = 0;
};
#endif

