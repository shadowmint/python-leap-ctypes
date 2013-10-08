#include <stdio.h>
#include <Leap.h>
#include <cleap.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// DEBUG mode: randomly generate frames (for when no leaps plugged in)
#define DEBUG 0

// Simplifies code
using namespace Leap;

// Always use this listener for code
class CleapListener : public Listener {
  public:
    struct leap_listener *listener;
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void convertVector(Vector &src, struct leap_vector *target);
    virtual void convertFrame(struct leap_event *event, Frame &frame);
    virtual void convertFake(struct leap_event *event); // Testing
};

// Reserve a slot and return the event for it.
// This also enqueues the event, so the return can be discarded.
struct leap_event *leap_listener_event(struct leap_listener *listener, int event_code);

// Reserve a slot for the next event and return the id.
// If no event is available, discard the oldest.
int leap_listener_reserve(struct leap_listener *listener);

// Return the id of the currently oldest event
int leap_listener_oldest(struct leap_listener *listener);

// Discard the oldest event and return the slot to the pool
void leap_listener_consume_oldest(struct leap_listener *listener);

// Resolve circular offsets
int leap_listener_resolve(struct leap_listener *listener, int offset);

// Random numbers
int leap_random(void);

void CleapListener::onInit(const Controller& controller) {
  leap_listener_event(this->listener, LEAP_ON_INIT);
}

void CleapListener::onConnect(const Controller& controller) {
  leap_listener_event(this->listener, LEAP_ON_CONNECT);
}

void CleapListener::onDisconnect(const Controller& controller) {
  leap_listener_event(this->listener, LEAP_ON_DISCONNECT);
}

void CleapListener::onExit(const Controller& controller) {
 leap_listener_event(this->listener, LEAP_ON_EXIT);
}

void CleapListener::onFrame(const Controller& controller) {
  //Frame frame;
  struct leap_event *event = leap_listener_event(this->listener, LEAP_ON_FRAME);
#if DEBUG
  this->convertFake(event);
#else
  Frame frame = controller.frame();
  this->convertFrame(event, frame);
#endif
}

void CleapListener::onFocusGained(const Controller& controller) {
  leap_listener_event(this->listener, LEAP_ON_FOCUS_GAINED);
}

void CleapListener::onFocusLost(const Controller& controller) {
  leap_listener_event(this->listener, LEAP_ON_FOCUS_LOST);
}

void CleapListener::convertVector(Vector &src, struct leap_vector *target) {
  target->points[0] = src.x;
  target->points[1] = src.y;
  target->points[2] = src.z;
}

void CleapListener::convertFake(struct leap_event *event) {
  struct leap_frame *frame = &event->frame;
  frame->id = leap_random();
  frame->timestamp = (long) time(NULL);
  frame->hand_count = leap_random() % 3;
  int i;
  for (i = 0; i < frame->hand_count; i++) {
    frame->hands[i].finger_count = leap_random() % 6;
  }
}

void CleapListener::convertFrame(struct leap_event *event, Frame &frame_) {
  int i, j;
  Hand hand_;
  FingerList fingers;
  InteractionBox bounds_;
  struct leap_hand *hand;
  struct leap_frame *frame = &event->frame;
  struct leap_bounds *bounds = &event->frame.bounds;

  // Frame
  frame->id = frame_.id();
  frame->timestamp = frame_.timestamp();
  frame->hand_count = frame_.hands().count();

  // We don't support arbitrary hand counts in this binding.
  if (frame->hand_count > 2) 
    frame->hand_count = 2;

  // Interaction box
  bounds_ = frame_.interactionBox();
  Vector bounds_center = bounds_.center();
  this->convertVector(bounds_center, &bounds->center);
  bounds->size.points[0] = bounds_.width();
  bounds->size.points[1] = bounds_.height();
  bounds->size.points[2] = bounds_.depth();

  // Hands
  for (i = 0; i < frame->hand_count; ++i) {
    hand_ = frame_.hands()[i];
    hand = &frame->hands[i];
    hand->sphere_radius = hand_.sphereRadius();
    Vector palm_position = hand_.palmPosition();
    Vector palm_normal = hand_.palmNormal();
    Vector direction = hand_.direction();
    this->convertVector(palm_position, &hand->palm_position);
    this->convertVector(palm_normal, &hand->palm_normal);
    this->convertVector(direction, &hand->direction);

    // Fingers
    hand->finger_count = hand_.fingers().count();
    if (hand->finger_count > 0) {
      fingers = hand_.fingers();
      for (j = 0; j < hand->finger_count; j++) {
        Vector finger_position = fingers[j].tipPosition();
        Vector finger_velocity = fingers[j].tipVelocity();
        Vector finger_direction = fingers[j].direction();
        this->convertVector(finger_position, &hand->fingers[j].position);
        this->convertVector(finger_velocity, &hand->fingers[j].velocity);
        this->convertVector(finger_direction, &hand->fingers[j].direction);
        hand->fingers[j].id = fingers[i].id();
      }
    }
  }
}

struct leap_controller *leap_controller() {
  Controller *controller = new Controller();
  struct leap_controller *rtn = (struct leap_controller *) calloc(1, sizeof(struct leap_controller));
  rtn->data = controller;
  return rtn;
}

void leap_enable_background(struct leap_controller *controller) {
  Controller *impl = (Controller *) controller->data;
  impl->setPolicyFlags(Controller::POLICY_BACKGROUND_FRAMES);
}

void leap_controller_dispose(struct leap_controller *controller) {
  Controller *impl = (Controller *) controller->data;
  delete impl;
  free(controller);
}

struct leap_listener *leap_listener(int size) {

  int i;
  CleapListener *listener = new CleapListener();
  struct leap_listener *rtn = (struct leap_listener *) calloc(1, sizeof(struct leap_listener));

  listener->listener = rtn;

  rtn->data = listener;
  rtn->event_offset = 0;
  rtn->event_count = 0;
  rtn->event_buffer = size;
  rtn->events = (struct leap_event **) calloc(size, sizeof(struct leap_event *));
  for (i = 0; i < size; i++) {
    rtn->events[i] = (struct leap_event *) calloc(1, sizeof(struct leap_event));
  }

  return rtn;
}

int leap_listener_resolve(struct leap_listener *listener, int offset) {
  int id = listener->event_offset + offset;
  if (id >= listener->event_buffer) 
    id -= listener->event_buffer;
  return id;
}

int leap_listener_reserve(struct leap_listener *listener) {
  int rtn;
  if (listener->event_count == listener->event_buffer) {
    rtn = listener->event_offset;
    listener->event_offset = leap_listener_resolve(listener, 1);
  }
  else {
    ++listener->event_count;
    rtn = leap_listener_resolve(listener, listener->event_count - 1);
  }
  return rtn;
}

struct leap_event *leap_listener_event(struct leap_listener *listener, int event_code) {
  int id = leap_listener_reserve(listener);
  struct leap_event *rtn = listener->events[id];
  rtn->event_code = event_code;
  return rtn;
}

int leap_listener_oldest(struct leap_listener *listener) {
  return listener->event_offset;
}

void leap_listener_consume_oldest(struct leap_listener *listener) {
  if (listener->event_count > 0) {
    listener->event_offset = leap_listener_resolve(listener, 1);
    --listener->event_count;
  }
}

void leap_listener_dispose(struct leap_listener *listener) {

  int i;
  struct leap_event *event;
  CleapListener *impl = (CleapListener *) listener->data;
  
  // Drop events
  for (i = 0; i < listener->event_buffer; i++) {
    free(listener->events[i]);
  }
  free(listener->events);
  listener->events = NULL;

  delete impl;
  free(listener);
}

void leap_add_listener(struct leap_controller *controller, struct leap_listener *listener) {
  Controller *cimpl = (Controller *) controller->data;
  CleapListener *impl = (CleapListener *) listener->data;
  cimpl->addListener(*impl);
}

void leap_remove_listener(struct leap_controller *controller, struct leap_listener *listener) {
  Controller *cimpl = (Controller *) controller->data;
  CleapListener *impl = (CleapListener *) listener->data;
  cimpl->removeListener(*impl);
}

struct leap_event *leap_poll_listener(struct leap_listener *listener) {
  int id;
  struct leap_event *rtn = NULL;
#if DEBUG
  int i; // Create random events in debug mode.
  Controller controller;
  CleapListener *impl = (CleapListener *) listener->data;
  if ((listener->event_count == 0) && (leap_random() % 2 == 0)) {
    for (i = 0; i < 150; i++) {
      impl->onFrame(controller);
    }
  }
#endif
  if (listener->event_count > 0) {
    id = leap_listener_oldest(listener);
    memcpy(&listener->event, listener->events[id], sizeof(struct leap_event));
    leap_listener_consume_oldest(listener);
    rtn = &listener->event;
  }
  return rtn;
}

void leap_event_dispose(struct leap_event *event) {
  CleapListener *impl = (CleapListener *) event->listener->data;
}

int leap_random(void) {
#ifdef WIN32
  return rand();
#else
  return random();
#endif
}
