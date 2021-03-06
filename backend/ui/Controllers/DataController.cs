using System;
using System.Collections.Generic;
using System.Linq;
using Lockbase.CoreDomain;
using Lockbase.CoreDomain.Aggregates;
using Lockbase.CoreDomain.Entities;
using Lockbase.CoreDomain.Enumerations;
using Lockbase.CoreDomain.Extensions;
using Lockbase.CoreDomain.Services;
using Lockbase.CoreDomain.ValueObjects;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using ui.Models;

namespace ui.Controllers
{
    [Route("api/[controller]")]
	public class DataController : Controller
	{
		private const string TOPIC_RESPONSE = "response";
		private readonly AtomicValue<LockSystem> lockSystem;
		private readonly ILogger<DataController> logger;
		private readonly IObserver<Statement> statementObserver;
		private readonly Id idGenerator;

		public DataController(
			AtomicValue<LockSystem> lockSystem, 
			ILogger<DataController> logger, 
			IObserver<Statement> statementObserver,
			Id idGenerator)
		{
			this.lockSystem = lockSystem;
			this.logger = logger;
			this.statementObserver = statementObserver;
			this.idGenerator = idGenerator;
		}

		// https://localhost:5001/api/data/check?keyId=233&lockid=34434&dateTime=2010-12-09T08:00:00.000Z	
		[HttpGet("[action]")]
		public bool Check(string keyId, string lockId, DateTime dateTime) {
			dateTime = dateTime.ToUniversalTime();
			logger.LogInformation("key {Key} , Lock {Lock}, DateTime {DateTime}", keyId, lockId, dateTime);

			LockSystem system = lockSystem; 

			Key key = system.QueryKey(keyId);
			Lock @lock = system.QueryLock(lockId);

            var @event = new Event(this.idGenerator.NewId(TableIds.Event, system.Events.Count()+1), 
				dateTime, key, @lock, system.HasAccess(key, @lock, dateTime));

			// statementObserver.OnNext(new Statement(TOPIC_RESPONSE, 4711, 
			// 	$"EK,{@event.Lock.Id},{@event.Key.Id},{@event.IsOpen}"));

			lockSystem.SetValue( x => x.AddEvent(@event));
			return @event.EventType == EventType.Authorized_Access;
		}

		[HttpGet("[action]")]
		public IEnumerable<PersonInfo> Persons()
		{
			logger.LogInformation("Persons called");

			LockSystem system = lockSystem; 
			if (system.Keys.IsEmpty()) {
				yield break; 
			}
			else 
			{
				Func<string, string> GetId = 
					x => system.Keys.SingleOrDefault( y => y.ExtData.Contains(x))?.Id;

				yield return new PersonInfo { keyId = GetId("900-1"), 
					value = "900-1", label = "Ahrens, Andrea", department = "Geschäftsführung", color = "OliveDrab" };
				yield return new PersonInfo { keyId = GetId("901-1"), 
					value = "901-1", label = "Barthauer, Thomas", department = "Geschäftsführung", color = "SandyBrown" };
				yield return new PersonInfo { keyId = GetId("103-1"), 
					value = "103-1", label = "Fendler, Klaus", department = "Buchhaltung" };
				yield return new PersonInfo { keyId = GetId("104-1"), 
					value = "104-1", label = "Kistler, Sabine", department = "Vertrieb" };
				yield return new PersonInfo { keyId = GetId("105-1"),
					value = "105-1", label = "Kohl, Ulrich", department = "Vertrieb" };
				yield return new PersonInfo { keyId = GetId("200-1"),
					value = "200-1", label = "Leinkamp, Sebastian", department = "Lager" };
				yield return new PersonInfo { keyId = GetId("201-1"),
					value = "201-1", label = "Mertens, Martina", department = "Lager" };
				yield return new PersonInfo { keyId = GetId("202-1"),
					value = "202-1", label = "Sidow, Janin", department = "Montage" };
				yield return new PersonInfo { keyId = GetId("203-1"),
					value = "203-1", label = "Walter, Jens", department = "Montage" };
				yield return new PersonInfo { keyId = GetId("203-2"),
					value = "203-2", label = "Winter, Sina", department = "Montage" };
				yield return new PersonInfo { keyId = GetId("203-3"),
					value = "203-3", label = "Wondraschek, Volker", department = "Montage" };
			}
		}

		[HttpGet("[action]")]
		public IEnumerable<GroupOrDoor> Doors()
		{
			logger.LogInformation("Doors called");

			LockSystem system = lockSystem;
			if (system.Locks.IsEmpty()) {
				yield break; 
			}
			else 
			{
				Func<string, string> GetId = x => system.Locks.SingleOrDefault( y => y.ExtData.Contains(x))?.Id;

				yield return new DoorInfo { lockId = GetId("W1"), 
					value = "W1", label = "Tor West", building = "-.-", image = "tor_west", color = "LightSlateGray" };

				yield return new GroupInfo {
					type = "group", name = "Verwaltung", items = new []{
						new DoorInfo { lockId =  GetId("100"), 
							value = "100", label = "Konferenzraum", building = "Verwaltung", image = "konferenzraum", color = "Yellow" },
						new DoorInfo { lockId =  GetId("101"), 
							value = "101", label = "Büro Ahrens", building = "Verwaltung", image = "buero_ahrens", color = "Yellow" },
						new DoorInfo { lockId =  GetId("102"), 
							value = "102", label = "Büro Barthauer", building = "Verwaltung", image = "buero_barthauer", color = "Yellow" },
						new DoorInfo { lockId =  GetId("103"), 
							value = "103", label = "Buchhaltung", building = "Verwaltung", image = "buchhaltung", color = "Yellow" },
						new DoorInfo { lockId =  GetId("104"), 
							value = "104", label = "Büro Vertrieb 1", building = "Verwaltung", image = "buero_vertrieb1", color = "Yellow" },
						new DoorInfo { lockId =  GetId("105"), 
							value = "105", label = "Büro Vertrieb 2", building = "Verwaltung", image = "buero_vertrieb2", color = "Yellow" },
						new DoorInfo { lockId = GetId("Z1"), 
							value = "Z1", label = "Eingang West", building = "Verwaltung", image = "eingang_west", color = "Yellow"  }
					}
				};

				yield return new GroupInfo {
					type = "group", name = "Produktion", items = new []{
						new DoorInfo { lockId =  GetId("204"), 
							value = "204", label = "Werkhalle West", building = "Produktion", image = "werkhalle_west", color = "DeepSkyBlue" },
						new DoorInfo { lockId =  GetId("200"), 
							value = "200", label = "Metalllager", building = "Produktion", image = "metalllager", color = "DeepSkyBlue" },
						new DoorInfo { lockId =  GetId("202"), 
							value = "202", label = "Büro Montage", building = "Produktion", image = "buero_montage", color = "DeepSkyBlue" },
						new DoorInfo { lockId =  GetId("201"), 
							value = "201", label = "Warenlager", building = "Produktion", image = "warenlager", color = "DeepSkyBlue" },
						new DoorInfo { lockId =  GetId("205"), 
							value = "205", label = "Werkhalle Süd", building = "Produktion", image = "werkhalle_sued", color = "DeepSkyBlue" }
					}
				};
			}
		}
	}
}
