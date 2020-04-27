template<class infoType>
struct node
{
    infoType * info;
    node<infoType> *next;

    // Destructor
    // Delete deallocate information and unlink
    ~node();
};

template<class infoType>
node<infoType>::~node()
{
    delete info;
    next = nullptr;
}