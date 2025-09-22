#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp> 
#include <boost/multi_index_container.hpp>
#include <boost/container/string.hpp> 
#include <boost/multi_index/member.hpp> 
#include <boost/multi_index/ordered_index.hpp>

using namespace boost::interprocess; 
namespace bmi = boost::multi_index; 

using char_allocator = managed_shared_memory::allocator<char>::type; 
using shm_string = boost::container::basic_string<char, std::char_traits<char>, char_allocator>; 


struct Employee
{
    int id; 
    int age; 
    shm_string name; 
    Employee(int id_, int age_, const char* name_, const char_allocator& alloc) : 
        id(id_), 
        age(age_), 
        name(name_, alloc)
    {}
}; 

/* Tags */
struct id
{}; 
struct age
{}; 
struct name
{};

/* 
* Define a multi_index_container of employees with following indices: 
* -> a unique index sorted by employee::int
* -> a non-unique index sorted by employee::name
* -> a non->unique index sorted by employee::age
*/

using EmployeeSet = bmi::multi_index_container<Employee, 
    bmi::indexed_by<
        bmi::ordered_unique<bmi::tag<id>, bmi::member<Employee, int, &Employee::id>>, 
    bmi::ordered_non_unique<bmi::tag<name>, bmi::member<Employee, shm_string, &Employee::name>>, 
    bmi::ordered_non_unique<bmi::tag<age>, bmi::member<Employee, int, &Employee::age>>>, 
    managed_shared_memory::allocator<Employee>::type>; 

int main()
{
    struct shm_remover
    {
        shm_remover(){ shared_memory_object::remove("MyName"); }
        ~shm_remover(){ shared_memory_object::remove("MyName"); }
    } remover; 

    managed_shared_memory segment(create_only, "MyName", 65536); 
    EmployeeSet* es = segment.construct<EmployeeSet>("MyMultiIndexContainer")
                                        (EmployeeSet::ctor_args_list(), segment.get_allocator<Employee>()); 

    char_allocator ca(segment.get_allocator<char>()); 
    es->insert(Employee(0, 31, "Joe", ca)); 
    es->insert(Employee(1, 27, "Robert", ca)); 
    es->insert(Employee(2, 40, "John", ca)); 
    return 0; 
}
