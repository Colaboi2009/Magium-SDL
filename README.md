# Magium-SDL
 SDL3 version of Magium.

Note: most the files here and structure are for android.

The actual cpp files (which should be compilable with any platform) lie under ```com.Magium.Magium/app/jni/src```, and the resources (i.e. .magium files and pngs) are under ```com.Magium.Magium/app/src/main/assets```.

I will most probably later pull out the cpp files and resources and remove the android wrapper.

# Other versions of magium
There are the [magium-dev](https://github.com/thuiop/magium-dev) and [magium-recrystallized](https://github.com/Br3nnabee/magium-recrystallized) versions aswell and ofcourse the [original app](https://github.com/raduprv/Magium) made by Cristian Mihailescu, the original writer of magium.

I probably recommend either of the two remake versions if you are looking to contribute to magium, and not this repo since they do a much better job at documenting how you could contribute, but I'm just here to make a cross-platform version for my amusement :D.

Do also note that it doesn't really matter which remake you use to play new chapters, since we all use the same format for the text (the .magium files) which I came up with and all props go to [thiuop](https://github.com/thuiop) (creater of magium-dev) who actually implemented it and translated the original into.
This means that all versions should contain the same story once the new versions are released, and while it isn't quite as official yet, we will probably converge on a save format for files to both extract the saves of previous players from the original and to be able to use it on any version.
