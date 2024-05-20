# curly-potato
Learning experience with c++

# adding to number
Adding to a number for 100000 times via two threads should result in `200000` but 
without synchronisation and without atomic operations we get weird results.
```
╭[simmi@DESKTOP-SOLN47J] /mnt/…/software/peterson_lock
╰─> ./Main                                                                                                                                                                  on branch main
sum = 0
sum = 130945
╭[simmi@DESKTOP-SOLN47J] /mnt/…/software/peterson_lock
╰─> ./Main                                                                                                                                                                  on branch main
sum = 0
sum = 111299
╭[simmi@DESKTOP-SOLN47J] /mnt/…/software/peterson_lock
╰─> ./Main                                                                                                                                                                  on branch main
sum = 0
sum = 100000
╭[simmi@DESKTOP-SOLN47J] /mnt/…/software/peterson_lock
╰─> ./Main                                                                                                                                                                  on branch main
sum = 0
sum = 123588
╭[simmi@DESKTOP-SOLN47J] /mnt/…/software/peterson_lock
╰─> git add -A
```
