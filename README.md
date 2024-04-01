# stardew-valley-internal-hack
tested on stardew valley 1.6.3 build 24087
works in multi/single player

.

i found a mov instruction which runs in a loop and accesses to main player's gold

that function was loaded dynamically into memory

created a signature to find that func every time.

and created a codecave for getting money address in asm.

finally, i wrote a simple algorithm in cpp for get and set money.

# references
x64 hooking tutorial:
https://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html 
x64 inline assembly: https://www.unknowncheats.me/forum/1421944-post2.html

