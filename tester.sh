rm -rf .ghezi 
ghezi init
echo salam >a.txt
ghezi add a.txt
ghezi commit -m a_added
ghezi branch new_name
ghezi checkout new_name
echo bruh >b.txt
rm a.txt
ghezi add a.txt b.txt
ghezi commit -m b_added
ghezi merge master new_name
ghezi checkout master+new_name