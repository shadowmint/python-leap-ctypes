// Leap event codes
#define LEAP_ON_INIT 0x0001
#define LEAP_ON_CONNECT 0x0002
#define LEAP_ON_DISCONNECT 0x0003
#define LEAP_ON_EXIT 0x0004
#define LEAP_ON_FRAME 0x0005
#define LEAP_ON_FOCUS_GAINED 0x0006
#define LEAP_ON_FOCUS_LOST 0x0007

// Public C api
extern "C" 
{
  // Single 3D vector
  struct leap_vector {
    float points[3];
  };

  // Finger
  struct leap_finger {
    int id;
    struct leap_vector position;
    struct leap_vector velocity;
    struct leap_vector direction;
  };

  // Leap model hands
  struct leap_hand {
    int finger_count;
    struct leap_finger fingers[5];
    struct leap_vector palm_position;
    struct leap_vector palm_normal;
    struct leap_vector direction;
    float sphere_radius;
  };

  // Interaction area
  struct leap_bounds {
    struct leap_vector center;
    struct leap_vector size;
  };

  // Leap model frame
  struct leap_frame {
    long id;
    long timestamp;
    int hand_count;
    struct leap_hand hands[2];
    struct leap_bounds bounds;
  };

  // Public event struct
  struct leap_event {
    int event_code;
    struct leap_frame frame;
    struct leap_event *next;
    struct leap_listener *listener;
  };

  // Listener for leap events
  struct leap_listener {
    void *data;
    int event_buffer;
    int event_offset;
    int event_count;
    struct leap_event event;
    struct leap_event **events;
  };

  // Leap api controller
  struct leap_controller {
    void *data;
  };


  // Create a controller reference
  struct leap_controller *leap_controller();

  // Enable background tracking on a controller
  void leap_enable_background(struct leap_controller *controller);

  // Destroy controller
  void leap_controller_dispose(struct leap_controller *controller);

  // Create a listener
  // At most the most recent 'size' pending events will be stored.
  struct leap_listener *leap_listener(int size);

  // Destroy a listener
  void leap_listener_dispose(struct leap_listener *listener);

  // Start listening on a listener
  void leap_add_listener(struct leap_controller *controller, struct leap_listener *listener);

  // Stop listening on a listener
  void leap_remove_listener(struct leap_controller *controller, struct leap_listener *listener);

  // Poll for a events on a listener.
  // Returns NULL if no events are waiting.
  // The returned object will be valid until the next leap_poll_listener() call.
  struct leap_event *leap_poll_listener(struct leap_listener *listener);
}
