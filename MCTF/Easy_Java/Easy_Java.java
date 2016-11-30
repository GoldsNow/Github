import java.util.Scanner;

public class Easy_Java {
	
	public static boolean check(char [] ch){
		int [] chk = {40,3,46,40,84,31,48,45,19,25,66,14,31,15,74,20,20,61,25,23,28,88,54,31,5,69,10,23,29,53,29,2};
		
		for(int i = 0; i < ch.length; i++)
			if((int)ch[i] != chk[i])
				return false;
		return true;
	}
	
	public static char [] Swap(char [] ch){
		
		char [] c = new char[32];
		for(int i = 0 ;i * 4 < ch.length; i++)
		{
			c[i * 4 ] = (char)((int)ch[i * 4 + 3] + 1); 
			c[i * 4 + 3] = (char)((int)ch[i * 4 + 2] + 2); 
			c[i * 4 + 1] = (char)((int)ch[i * 4] + 3); 
			c[i * 4 + 2] = (char)((int)ch[i * 4 + 1] + 4); 
		}
		return c;
	}
	
	public static void main(String [] args){
		String xorkey = new String("Mirage_team");
		
		System.out.print("Welcome to MCTF!\nI should tell you loudly\nit's easy!!!!\nplease find the key:");
		
		Scanner scan = new Scanner(System.in);
		String flag = scan.next();
		
		char [] ch = flag.toCharArray();
		char [] xor = xorkey.toCharArray();
		
		for (int i =0;i<  ch.length;i++)
		{
			ch[i] ^= xor[i % xor.length];
		}
		
		if(ch[0] != 0 || ch[1] != 42 || ch[2] != 38 || ch[3] != 39 || Easy_Java.check(Easy_Java.Swap(ch))!=true)
		{	
			System.out.println("Sorry! Please Try again!");
		}
		else
		{
			System.out.println("Ok! You Got it!");
		}
	
		
	}
}
