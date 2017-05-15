#include "redcrane.hpp"

using namespace redc;

extern "C"
{
  void redc_text_draw(void *eng, const char *text)
  {
    auto rce = (redc::Engine*) eng;
    REDC_ASSERT_HAS_CLIENT(rce);

    rce->client->text_render->render_text(
            *rce->client->driver, std::string{text}, {0.0f, 0.0f}
    );
  }
}