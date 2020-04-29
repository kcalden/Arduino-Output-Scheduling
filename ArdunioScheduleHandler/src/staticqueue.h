/* Fixed size queue */

template<typename info_struct, unsigned int stack_size>
class StaticQueue
{
public:
    // Push info to the queue
    void push(info_struct &);

    // Remove info from the queue
    // Returns a reference to the info
    // WARNING:: Returns the last popped item if the queue is empty
    info_struct & pop();

    // Check the next item that can be popped from the queue
    // Returns constant reference to the info
    // WARNING:: Returns the last popped item if the queue is empty
    info_struct & peek();

    // Returns true if the queue is full
    inline bool full();

    // Return true if the queue is empty
    inline bool empty();

    // Return remaining space in the queue
    inline unsigned int itemsLeft();
private:
    unsigned int current_index = 0, last_index = 0, size = 0;
    info_struct queue[stack_size];
};

template<typename info_struct, unsigned int stack_size>
void StaticQueue<info_struct,stack_size>::push(info_struct & info)
{
    // Don't do anything if the queue is full
    if(size == stack_size) return;

    // If the queue had nothing in it push to the current index
    if(size==0) {
        queue[current_index] = info;
        last_index = current_index;
    }

    // Advance last index
    unsigned int new_last_index = (last_index+1) % stack_size;

    queue[new_last_index] = info;
    last_index = new_last_index;
    size++;
}

template<typename info_struct, unsigned int stack_size>
info_struct & StaticQueue<info_struct,stack_size>::pop()
{
    // Return the last popped index nothing is there
    if(size==0) return queue[current_index];
    
    // Temporarily store index of the thing we're popping
    unsigned int popped_index = current_index;

    // Advance the current index only if the queue isn't going to be empty
    if(current_index != last_index) 
    {
        unsigned int new_current_index = (current_index+1) % stack_size;
        current_index = new_current_index;
    }

    return queue[popped_index];
}

template<typename info_struct, unsigned int stack_size>
info_struct & StaticQueue<info_struct,stack_size>::peek()
{
    return queue[current_index];
}

template<typename info_struct, unsigned int stack_size>
inline bool StaticQueue<info_struct,stack_size>::full()
{
    return size == stack_size;
}

template<typename info_struct, unsigned int stack_size>
inline bool StaticQueue<info_struct,stack_size>::empty()
{
    return size == 0;
}

template<typename info_struct, unsigned int stack_size>
inline unsigned int StaticQueue<info_struct,stack_size>::itemsLeft()
{
    return stack_size - size;
}