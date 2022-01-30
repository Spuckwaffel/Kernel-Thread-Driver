# Kernel-Thread-Driver
This is a simple project of a driver + usermode.

Check out the UC post: https://www.unknowncheats.me/forum/general-programming-and-reversing/487919-kernel-thread-driver.html#post3358074

I decided to release my driver I used for quite a while (for BE and EAC games), especially for Fortnite, and it worked pretty well for me (I didn't share the driver).
I know kernel thread drivers are quite popular, but I wanted to contribute this to this awesome community, it wouldn't be possible without hundreds of older threads here that gave me the needed information.

Special Thanks to these people, I used some of their code/information in my project:
@zach898
@nbq
@Frostiest
@Swiftik
@ShoaShekelberg

The reason this driver is different from other kernel thread drivers, that it's not a full kernel cheat, it communicates with the usermode program too.

The driverentry takes 4 params:
NTSTATUS EntryPoint(ULONG64 mdl, ULONG64 code, ULONG64 output, ULONG64 PID)

mdl is the mdl pointer, because im not allocating a pool for my driver.
code saves the address what the current status code is. (0 = connected, 1 = success, 2 = error, 3 = disconnected, 4 = reading, 5 = getting base, 6 = init target process)
and output saves the address for the UM struct.

Basically it works like this:

I start the UM program.
Give the mapper the mdl pointer, code status address, the struct and usermode pid.
My kernel driver gets started and nulls the fps, creates a system thread and sets the status code to 0 (connected).
While this happens my UM programs stays in a loop until the status code is 0 and then it knows the kernel driver is running.
After that i send the target pid to my driver (set the code to 6, my kernel driver is in a permanent loop and always reads the status code and if it's 6, it saves the target pid.)
Then I can use basic stuff like reading memory and getting the base address.
For disconnecting the usermode, I send the code 3 to my driver, so it knows it should exit the system thread and won't read any more memory from our UM process.

Sidenote:
This kernel driver changes
InitialStack
VCreateTime
StartAddress
Win32StartAddress
KernelStack
CID
ExitStatus
from its current thread.

It supports 1909-21H1.

Obviously this isn't perfect, but it is good for beginners to learn from it.
This project shows that this forum is also great for information what the anticheats detect and how to bypass those checks.

The code might be messy, but oh well.
