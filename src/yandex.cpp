#include "yandex.h"
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

using namespace hcg001;

void Yandex::InitSdk()
{
	sky::Log("Yandex::InitSdk()");
#ifdef EMSCRIPTEN
	EM_ASM(
		var script = document.createElement('script');
		script.src = '/sdk.js';
		script.type = 'text/javascript';
		script.onload = function() {
			YaGames.init().then(ysdk => {
				window.ysdk = ysdk;
				window.ysdk.features.LoadingAPI.ready();

				window.ysdk.getLeaderboards().then(lb => {
					window.ylb = lb;
				});
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

void Yandex::SendHighScore(int value)
{
#ifdef EMSCRIPTEN
	EM_ASM({
		let score = $0;
		window.ylb.getLeaderboardPlayerEntry('score').then(res => {
			let prev = res.score;
			let send = prev < score;
			console.log('[Yandex::SendHighScore] prev:', prev, ", new: ", score, ", will_send: ", send);
			if (send) {
				window.ylb.setLeaderboardScore('score', score);
			}
		});
	}, value);
#endif
}
