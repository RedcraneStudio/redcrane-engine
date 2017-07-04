#include "../common/timed_text.h"
#include "redcrane.hpp"

using namespace redc;

extern "C"
{
  void redc_text_draw(void *eng, const char *text, float x, float y,
                      unsigned int side)
  {
    auto rce = (redc::Engine*) eng;
    REDC_ASSERT_HAS_CLIENT(rce);

    rce->client->text_render->render_text(
            *rce->client->driver, std::string{text}, {x, y},
            static_cast<Reference_Point>(side)
    );
  }

  void *redc_text_stream_new(void *eng, float timeout)
  {
    auto stream = new Timed_Text_Stream(timeout);
    return stream;
  }
  void redc_text_stream_delete(void *stream)
  {
    delete (Timed_Text_Stream*) stream;
  }
  void redc_text_stream_push_string(void *stream, const char *text)
  {
    auto text_stream = (Timed_Text_Stream*) stream;
    text_stream->push_line(text);
  }
  const char* redc_text_stream_full_text(void *stream)
  {
    auto text_stream = (Timed_Text_Stream*) stream;
    return strdup(text_stream->full_text().c_str());
  }
  void redc_text_stream_step(void *stream, float dt)
  {
    auto text_stream = (Timed_Text_Stream*) stream;
    text_stream->step(dt);
  }
}
