//package com.th000.keycontainer;

//import java.io.FileInputStream;
//import java.io.FileOutputStream;
//import android.content.Context;

//public class IOData {
//    public static String readFile(String path){
//        String filePath;
//        if(path == "CONFIG"){
//            filePath = Context.getFilesDir();
//        }
//        File file = new File(filePath + "/config/config.ini");
//        InputStream readStream = getContentResolver().openInputStream(
//            FileProvider.getUriForFile(this, DataManager.getFileProviderName(this), file));
//        String result = new BufferedReader(new InputStreamReader(readStream))
//        .lines().collect(Collectors.joining(System.lineSeparator()));
//        return result;
//    }
//}
