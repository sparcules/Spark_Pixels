package kc.spark.pixels.android.cloud.api;

import static org.solemnsilence.util.Py.truthy;
import android.os.Parcel;
import android.os.Parcelable;


public class FunctionResponse implements Parcelable {

	
	public static final int REQUEST_TYPE_FUNCTION = 1;
	public static final int REQUEST_TYPE_VARIABLE = 2;



	public final int requestType;
	public final String coreId;
	public final String responseValue;
	public final String responseType;
	public final boolean errorMakingRequest;


	public FunctionResponse(int requestType, String coreId, String responseType,
			String responseValue, boolean requestError) {
		this.requestType = requestType;
		this.coreId = coreId;
		this.responseType = responseType;
		this.responseValue = responseValue;
		this.errorMakingRequest = requestError;
	}

	public FunctionResponse(Parcel in) {
		this.requestType = in.readInt();
		this.coreId = in.readString();
		this.responseType = in.readString();
		this.responseValue = in.readString();
		this.errorMakingRequest = truthy(in.readInt());
	}

/*	@Override
	public String toString() {
		return "TinkerResponse [requestType=" + requestType + ", coreId=" + coreId + ", pin=" + pin
				+ ", responseValue=" + responseValue + ", responseType=" + responseType
				+ ", requestError=" + errorMakingRequest + "]";
	}
*/
	@Override
	public int describeContents() {
		return 0;
	}

	@Override
	public void writeToParcel(Parcel dest, int flags) {
		dest.writeInt(requestType);
		dest.writeString(coreId);
		dest.writeString(responseType);
		dest.writeString(responseValue);
		dest.writeInt((errorMakingRequest) ? 1 : 0);
	}


	public static final Parcelable.Creator<FunctionResponse> CREATOR = new Parcelable.Creator<FunctionResponse>() {

		@Override
		public FunctionResponse createFromParcel(Parcel in) {
			return new FunctionResponse(in);
		}

		@Override
		public FunctionResponse[] newArray(int size) {
			return new FunctionResponse[size];
		}
	};

}
