#include "yandex.h"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

using namespace hcg001;

void Yandex::InitSdk()
{
#ifdef EMSCRIPTEN
	EM_ASM(
		var script = document.createElement('script');
		script.src = '/sdk.js';
		script.type = 'text/javascript';
		script.onload = function() {
			YaGames.init().then(ysdk => {
				window.ysdk = ysdk;
				window.ysdk.features.LoadingAPI.ready();
			}).catch (error => {
				console.error("Yandex SDK: ", error);
			});
		};
		script.onerror = function() {
			console.error("Cannot load yandex SDK.");
		};
		document.head.appendChild(script);
	);
#endif
}

void Yandex::GameplayStart()
{
#ifdef EMSCRIPTEN
	EM_ASM(
		window.ysdk.features.GameplayAPI.start();
	);
#endif
}

void Yandex::GameplayStop()
{
#ifdef EMSCRIPTEN
	EM_ASM(
		window.ysdk.features.GameplayAPI.stop();
	);
#endif
}
